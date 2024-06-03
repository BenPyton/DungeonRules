// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	RuleTransitionNode.cpp
=============================================================================*/

#include "RuleTransitionNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleConduitNode.h"
#include "Kismet2/CompilerResultsLog.h"
#include "EdGraphUtilities.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "ScopedTransaction.h"
#include "UObject/UE5MainStreamObjectVersion.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRules.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

/////////////////////////////////////////////////////
// URuleTransitionNode

URuleTransitionNode::URuleTransitionNode()
	: Super()
{
}

void URuleTransitionNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
	Outputs->bHidden = true;
}

void URuleTransitionNode::PostPlacedNewNode()
{
	if (NodeInstance)
		return;

	CreateInstance();
}

void URuleTransitionNode::PostPasteNode()
{
	if (NodeInstance)
	{
		// Deep copy the pasted node instance
		CreateInstance(NodeInstance);
	}

	Super::PostPasteNode();

	// We don't want to paste nodes in that aren't fully linked (transition nodes have fixed pins as they
	// really describe the connection between two other nodes). If we find one missing link, get rid of the node.
	for(UEdGraphPin* Pin : Pins)
	{
		if(Pin->LinkedTo.Num() == 0)
		{
			DestroyNode();
			break;
		}
	}
}

#if WITH_EDITOR

void URuleTransitionNode::PostEditImport()
{
	ResetInstanceOwner();
#if false // TODO: move to URuleNodeBase
	if (NodeInstance)
	{
		InitializeInstance();
	}
#endif
}

void URuleTransitionNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetInstanceOwner();
}

#endif

void URuleTransitionNode::PostCopyNode()
{
	ResetInstanceOwner();
}

void URuleTransitionNode::ResetInstanceOwner()
{
	if (NodeInstance)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;

		NodeInstance->Rename(nullptr, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
		NodeInstance->ClearFlags(RF_Transient);
	}
}

FText URuleTransitionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	URuleNodeBase* PrevState = GetPreviousState();
	URuleNodeBase* NextState = GetNextState();

	if ((PrevState != NULL) && (NextState != NULL))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetStateName()));
		Args.Add(TEXT("NextState"), FText::FromString(NextState->GetStateName()));

		return FText::Format(LOCTEXT("PrevStateToNewState", "{PrevState} to {NextState}"), Args);
	}
	return FText::FromString(TEXT("NULL"));
}

FText URuleTransitionNode::GetTooltipText() const
{
	return LOCTEXT("StateTransitionTooltip", "This is a state transition");
}

URuleNodeBase* URuleTransitionNode::GetPreviousState() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<URuleNodeBase>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

URuleNodeBase* URuleTransitionNode::GetNextState() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<URuleNodeBase>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

FLinearColor URuleTransitionNode::GetNodeTitleColor() const
{
	return FColorList::Red;
}

void URuleTransitionNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if(UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void URuleTransitionNode::CreateConnections(URuleNodeBase* PreviousState, URuleNodeBase* NextState)
{
	// Previous to this
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	PreviousState->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(PreviousState->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	NextState->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(NextState->GetInputPin());
}

void URuleTransitionNode::RelinkHead(URuleNodeBase* NewTargetState)
{
	URuleNodeBase* TargetStateBeforeRelinking = GetNextState();

	// Relink the target state of the transition node
	Pins[1]->Modify();
	Pins[1]->BreakLinkTo(TargetStateBeforeRelinking->GetInputPin());
	Pins[1]->MakeLinkTo(NewTargetState->GetInputPin());
}

TArray<URuleTransitionNode*> URuleTransitionNode::GetListTransitionNodesToRelink(UEdGraphPin* SourcePin, UEdGraphPin* OldTargetPin, const TArray<UEdGraphNode*>& InSelectedGraphNodes)
{
	URuleNodeBase* SourceState = Cast<URuleNodeBase>(SourcePin->GetOwningNode());
	if (SourceState == nullptr || SourceState->GetInputPin() == nullptr || SourceState->GetOutputPin() == nullptr)
	{
		return {};
	}

	// Collect all transition nodes starting at the source state
	TArray<URuleTransitionNode*> TransitionNodeCandidates;
	SourceState->GetTransitionList(TransitionNodeCandidates);

	// Remove the transition nodes from the candidates that are linked to a different target state.
	for (int i = TransitionNodeCandidates.Num() - 1; i >= 0; i--)
	{
		URuleTransitionNode* CurrentTransition = TransitionNodeCandidates[i];

		// Get the actual target states from the transition nodes
		UEdGraphNode* TransitionTargetNode = CurrentTransition->GetNextState();
		URuleTransitionNode* CastedOldTarget = Cast<URuleTransitionNode>(OldTargetPin->GetOwningNode());
		UEdGraphNode* OldTargetNode = CastedOldTarget->GetNextState();

		// Compare the target states rather than comparing against the transition nodes
		if (TransitionTargetNode != OldTargetNode)
		{
			TransitionNodeCandidates.Remove(CurrentTransition);
		}
	}

	// Collect the subset of selected transitions from the list of possible transitions to be relinked
	TSet<URuleTransitionNode*> SelectedTransitionNodes;
	for (UEdGraphNode* GraphNode : InSelectedGraphNodes)
	{
		URuleTransitionNode* TransitionNode = Cast<URuleTransitionNode>(GraphNode);
		if (!TransitionNode)
		{
			continue;
		}

		if (TransitionNodeCandidates.Find(TransitionNode) != INDEX_NONE)
		{
			SelectedTransitionNodes.Add(TransitionNode);
		}
	}

	TArray<URuleTransitionNode*> Result;
	Result.Reserve(TransitionNodeCandidates.Num());
	for (URuleTransitionNode* TransitionNode : TransitionNodeCandidates)
	{
		// Only relink the selected transitions. If none are selected, relink them all.
		if (!SelectedTransitionNodes.IsEmpty() && SelectedTransitionNodes.Find(TransitionNode) == nullptr)
		{
			continue;
		}

		Result.Add(TransitionNode);
	}

	return Result;
}

void URuleTransitionNode::PrepareForCopying()
{
	Super::PrepareForCopying();
	if (NodeInstance)
	{
		// Temporarily take ownership of the node instance, so that it is not deleted when cutting
		NodeInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

FString URuleTransitionNode::GetStateName() const
{
	return TEXT("(null)");
}

FText URuleTransitionNode::GetConditionDescription() const
{
	return FText::FromString(TEXT("TODO"));
#if false // TODO: Node tooltips
	if (!Condition)
		return NSLOCTEXT("DungeonRules", "URuleTransitionNode_NoCondition", "Always true.");

	return Condition->GetDescription();
#endif
}

void URuleTransitionNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FAnimPhysObjectVersion::GUID);
	Ar.UsingCustomVersion(FUE5MainStreamObjectVersion::GUID);
}

void URuleTransitionNode::CreateInstance(const UDungeonRuleTransition* Template)
{
	UEdGraph* Graph = GetGraph();
	UObject* GraphOwner = Graph ? Graph->GetOuter() : nullptr;
	if (!GraphOwner)
		return;

	if (!Template)
	{
		NodeInstance = NewObject<UDungeonRuleTransition>(GraphOwner);
		NodeInstance->SetFlags(RF_Transactional);
	}
	else
	{
		NodeInstance = DuplicateObject(Template, GraphOwner);
	}
}

#undef LOCTEXT_NAMESPACE
