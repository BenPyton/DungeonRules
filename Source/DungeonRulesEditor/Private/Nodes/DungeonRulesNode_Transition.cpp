// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

/*=============================================================================
	DungeonRulesNode_Transition.cpp
=============================================================================*/

#include "DungeonRulesNode_Transition.h"
#include "DungeonRulesNode_Conduit.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRules.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

/////////////////////////////////////////////////////
// UDungeonRulesNode_Transition

UDungeonRulesNode_Transition::UDungeonRulesNode_Transition()
	: Super()
{
}

void UDungeonRulesNode_Transition::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
	Outputs->bHidden = true;
}

void UDungeonRulesNode_Transition::PostPasteNode()
{
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

FText UDungeonRulesNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UDungeonRulesNode* PrevState = GetPreviousState();
	UDungeonRulesNode* NextState = GetNextState();

	if ((PrevState != NULL) && (NextState != NULL))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetStateName()));
		Args.Add(TEXT("NextState"), FText::FromString(NextState->GetStateName()));

		return FText::Format(LOCTEXT("PrevStateToNewState", "{PrevState} to {NextState}"), Args);
	}
	return Super::GetNodeTitle(TitleType);
}

const UClass* UDungeonRulesNode_Transition::GetInstanceClass() const
{
	return UDungeonRuleTransition::StaticClass();
}

FString UDungeonRulesNode_Transition::GetStateName() const
{
	return TEXT("(null)");
}

TArray<FName> UDungeonRulesNode_Transition::GetPropertyNamesToEdit() const
{
	return {
		GET_MEMBER_NAME_CHECKED(UDungeonRuleTransition, PriorityOrder),
		GET_MEMBER_NAME_CHECKED(UDungeonRuleTransition, Condition)
	};
}

UDungeonRulesNode* UDungeonRulesNode_Transition::GetPreviousState() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UDungeonRulesNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

UDungeonRulesNode* UDungeonRulesNode_Transition::GetNextState() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UDungeonRulesNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

FLinearColor UDungeonRulesNode_Transition::GetNodeTitleColor() const
{
	return FColorList::Red;
}

void UDungeonRulesNode_Transition::PinConnectionListChanged(UEdGraphPin* Pin)
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

void UDungeonRulesNode_Transition::CreateConnections(UDungeonRulesNode* PreviousState, UDungeonRulesNode* NextState)
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

void UDungeonRulesNode_Transition::RelinkHead(UDungeonRulesNode* NewTargetState)
{
	UDungeonRulesNode* TargetStateBeforeRelinking = GetNextState();

	// Relink the target state of the transition node
	Pins[1]->Modify();
	Pins[1]->BreakLinkTo(TargetStateBeforeRelinking->GetInputPin());
	Pins[1]->MakeLinkTo(NewTargetState->GetInputPin());
}

TArray<UDungeonRulesNode_Transition*> UDungeonRulesNode_Transition::GetListTransitionNodesToRelink(UEdGraphPin* SourcePin, UEdGraphPin* OldTargetPin, const TArray<UEdGraphNode*>& InSelectedGraphNodes)
{
	UDungeonRulesNode* SourceState = Cast<UDungeonRulesNode>(SourcePin->GetOwningNode());
	if (SourceState == nullptr || SourceState->GetInputPin() == nullptr || SourceState->GetOutputPin() == nullptr)
	{
		return {};
	}

	// Collect all transition nodes starting at the source state
	TArray<UDungeonRulesNode_Transition*> TransitionNodeCandidates;
	SourceState->GetTransitionList(TransitionNodeCandidates);

	// Remove the transition nodes from the candidates that are linked to a different target state.
	for (int i = TransitionNodeCandidates.Num() - 1; i >= 0; i--)
	{
		UDungeonRulesNode_Transition* CurrentTransition = TransitionNodeCandidates[i];

		// Get the actual target states from the transition nodes
		UEdGraphNode* TransitionTargetNode = CurrentTransition->GetNextState();
		UDungeonRulesNode_Transition* CastedOldTarget = Cast<UDungeonRulesNode_Transition>(OldTargetPin->GetOwningNode());
		UEdGraphNode* OldTargetNode = CastedOldTarget->GetNextState();

		// Compare the target states rather than comparing against the transition nodes
		if (TransitionTargetNode != OldTargetNode)
		{
			TransitionNodeCandidates.Remove(CurrentTransition);
		}
	}

	// Collect the subset of selected transitions from the list of possible transitions to be relinked
	TSet<UDungeonRulesNode_Transition*> SelectedTransitionNodes;
	for (UEdGraphNode* GraphNode : InSelectedGraphNodes)
	{
		UDungeonRulesNode_Transition* TransitionNode = Cast<UDungeonRulesNode_Transition>(GraphNode);
		if (!TransitionNode)
		{
			continue;
		}

		if (TransitionNodeCandidates.Find(TransitionNode) != INDEX_NONE)
		{
			SelectedTransitionNodes.Add(TransitionNode);
		}
	}

	TArray<UDungeonRulesNode_Transition*> Result;
	Result.Reserve(TransitionNodeCandidates.Num());
	for (UDungeonRulesNode_Transition* TransitionNode : TransitionNodeCandidates)
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

void UDungeonRulesNode_Transition::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FAnimPhysObjectVersion::GUID);
	Ar.UsingCustomVersion(FUE5MainStreamObjectVersion::GUID);
}

#undef LOCTEXT_NAMESPACE
