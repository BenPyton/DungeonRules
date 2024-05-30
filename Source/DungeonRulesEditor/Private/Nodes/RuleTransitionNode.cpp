// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	RuleTransitionNode.cpp
=============================================================================*/

#include "RuleTransitionNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
//#include "Animation/AnimInstance.h"
//#include "DungeonRulesGraph.h"
//#include "DungeonRulesTransitionGraph.h"
//#include "DungeonRulesTransitionSchema.h"
//#include "AnimGraphNode_StateResult.h"
//#include "AnimGraphNode_TransitionResult.h"
#include "RuleConduitNode.h"
#include "Kismet2/CompilerResultsLog.h"
#include "EdGraphUtilities.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "ScopedTransaction.h"
#include "UObject/UE5MainStreamObjectVersion.h"
#include "DungeonRulesEdTypes.h"

//////////////////////////////////////////////////////////////////////////
// IRuleTransitionNodeSharedDataHelper

#define LOCTEXT_NAMESPACE "A3Nodes"

#if false
class PROCEDURALDUNGEONEDITOR_API IRuleTransitionNodeSharedDataHelper
{
public:
	void UpdateSharedData(URuleTransitionNode* Node, TSharedPtr<INameValidatorInterface> NameValidator);
	void MakeSureGuidExists(URuleTransitionNode* Node);

protected:
	virtual bool CheckIfNodesShouldShareData(const URuleTransitionNode* NodeA, const URuleTransitionNode* NodeB) = 0;
	virtual bool CheckIfHasDataToShare(const URuleTransitionNode* Node) = 0;
	virtual void ShareData(URuleTransitionNode* NodeWhoWantsToShare, const URuleTransitionNode* ShareFrom) = 0;
	virtual FString& AccessShareDataName(URuleTransitionNode* Node) = 0;
	virtual FGuid& AccessShareDataGuid(URuleTransitionNode* Node) = 0;
};

//////////////////////////////////////////////////////////////////////////
// FRuleTransitionNodeSharedRulesHelper

class PROCEDURALDUNGEONEDITOR_API FRuleTransitionNodeSharedRulesHelper : public IRuleTransitionNodeSharedDataHelper
{
protected:
	virtual bool CheckIfNodesShouldShareData(const URuleTransitionNode* NodeA, const URuleTransitionNode* NodeB) override;
	virtual bool CheckIfHasDataToShare(const URuleTransitionNode* Node) override;
	virtual void ShareData(URuleTransitionNode* NodeWhoWantsToShare, const URuleTransitionNode* ShareFrom) override;
	virtual FString& AccessShareDataName(URuleTransitionNode* Node) override;
	virtual FGuid& AccessShareDataGuid(URuleTransitionNode* Node) override;
};
#endif

/////////////////////////////////////////////////////
// URuleTransitionNode

URuleTransitionNode::URuleTransitionNode()
	: Super()
{
#if false
	bSharedRules = false;
	SharedRulesGuid.Invalidate();
#endif
	PriorityOrder = 1;
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
#if false
	CreateBoundGraph();
#endif
}

void URuleTransitionNode::PostLoad()
{
	Super::PostLoad();

#if false
	// make sure we have guid for shared rules 
	if (bSharedRules && !SharedRulesGuid.IsValid())
	{
		FRuleTransitionNodeSharedRulesHelper().MakeSureGuidExists(this);
	}
#endif
}

void URuleTransitionNode::PostPasteNode()
{
#if false
	if (bSharedRules)
	{
		FRuleTransitionNodeSharedRulesHelper().UpdateSharedData(this, MakeShareable(new FRuleTransitionNodeSharedRulesNameValidator(this)));
	}

	if (BoundGraph == NULL)
	{
		// fail-safe, create empty transition graph
		CreateBoundGraph();
	}

	for (UEdGraphNode* GraphNode : BoundGraph->Nodes)
	{
		GraphNode->CreateNewGuid();
		GraphNode->PostPasteNode();
		GraphNode->ReconstructNode();
	}
#endif

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

FText URuleTransitionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	URuleNodeBase* PrevState = GetPreviousState();
	URuleNodeBase* NextState = GetNextState();

#if false
	if (!SharedRulesName.IsEmpty())
	{
		return FText::FromString(SharedRulesName);
	}
	else
#endif
	if ((PrevState != NULL) && (NextState != NULL))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetStateName()));
		Args.Add(TEXT("NextState"), FText::FromString(NextState->GetStateName()));

		return FText::Format(LOCTEXT("PrevStateToNewState", "{PrevState} to {NextState}"), Args);
	}
#if false
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("BoundGraph"), (BoundGraph != NULL) ? FText::FromString(BoundGraph->GetName()) : LOCTEXT("Null", "(null)") );
		// @TODO: FText::Format() is slow, and we could benefit from caching 
		//        this off like we do for a lot of other nodes (but we have to
		//        make sure to invalidate the cached string at the appropriate 
		//        times).
		return FText::Format(LOCTEXT("TransitioNState", "Trans {BoundGraph}}"), Args);
	}
#endif
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
	URuleNodeBase* SourceState = GetPreviousState();
	URuleNodeBase* TargetStateBeforeRelinking = GetNextState();

	// Remove the incoming transition from the previous target state
	TargetStateBeforeRelinking->GetInputPin()->Modify();
	TargetStateBeforeRelinking->GetInputPin()->BreakLinkTo(SourceState->GetOutputPin());

	// Add the new incoming transition to the new target state
	NewTargetState->GetInputPin()->Modify();
	NewTargetState->GetInputPin()->MakeLinkTo(SourceState->GetOutputPin());

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
#if false
	// move bound graph node here, so during copying it will be referenced
	// for shared nodes at least one of them has to be referencing it, so we will be fine
	BoundGraph->Rename(NULL, this, REN_DoNotDirty | REN_DontCreateRedirectors);
#endif
}

void URuleTransitionNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
#if false
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(URuleTransitionNode, CrossfadeDuration) || 
		PropertyName == GET_MEMBER_NAME_CHECKED(URuleTransitionNode, BlendMode) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(URuleTransitionNode, CustomBlendCurve) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(URuleTransitionNode, BlendProfile))
	{
		PropagateCrossfadeSettings();
	}

	if (PropertyName == FName(TEXT("LogicType")) )
	{
		if ((LogicType == ETransitionLogicType::TLT_Custom) && (CustomTransitionGraph == NULL))
		{
			CreateCustomTransitionGraph();
		}
		else if (CustomTransitionGraph != NULL)
		{
			// UAnimationCustomTransitionSchema::HandleGraphBeingDeleted resets logic type, so we'll need to restore it after RemoveGraph
			const TEnumAsByte<ETransitionLogicType::Type> DesiredLogicType = LogicType;

			UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
			FBlueprintEditorUtils::RemoveGraph(Blueprint, CustomTransitionGraph);
			CustomTransitionGraph = NULL;

			LogicType = DesiredLogicType;
		}
	}
#endif

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FString URuleTransitionNode::GetStateName() const
{
#if false
	return (BoundGraph != NULL) ? *(BoundGraph->GetName()) : TEXT("(null)");
#else
	return TEXT("(null)");
#endif
}

#if false
void URuleTransitionNode::MakeRulesShareable(FString ShareName)
{
	bSharedRules = true;
	SharedRulesName = ShareName;
	SharedRulesGuid = FGuid::NewGuid();
}

void URuleTransitionNode::UnshareRules()
{
	bSharedRules = false;	
	SharedRulesName.Empty();
	SharedRulesGuid.Invalidate();

	if ((BoundGraph == NULL) || IsBoundGraphShared())
	{
		BoundGraph = NULL;
		CreateBoundGraph();
	}
}

void URuleTransitionNode::UseSharedRules(const URuleTransitionNode* Node)
{
	if(Node == this || Node == nullptr)
	{
		return;
	}

	FScopedTransaction Transaction(LOCTEXT("UseSharedRules", "Use Shared Rules"));

	Modify();

	UEdGraph* CurrentGraph = GetGraph();
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(CurrentGraph);

	UEdGraph* GraphToDelete = NULL;
	if ((BoundGraph != NULL) && !IsBoundGraphShared())
	{
		GraphToDelete = BoundGraph;
	}

	BoundGraph = Node->BoundGraph;
	bSharedRules = Node->bSharedRules;
	SharedRulesName = Node->SharedRulesName;
	SharedColor = Node->SharedColor;
	SharedRulesGuid = Node->SharedRulesGuid;

	if (GraphToDelete != NULL)
	{
		FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToDelete);
	}
}
#endif

#if false
void URuleTransitionNode::CreateBoundGraph()
{
	// Create a new animation graph
	check(BoundGraph == NULL);
	BoundGraph = FBlueprintEditorUtils::CreateNewGraph(this, NAME_None, UDungeonRulesTransitionGraph::StaticClass(), UDungeonRulesTransitionSchema::StaticClass());
	check(BoundGraph);

	// Find an interesting name
	FEdGraphUtilities::RenameGraphToNameOrCloseToName(BoundGraph, TEXT("Transition"));

	// Initialize the anim graph
	const UEdGraphSchema* Schema = BoundGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*BoundGraph);

	// Add the new graph as a child of our parent graph
	UEdGraph* ParentGraph = GetGraph();

	if(ParentGraph->SubGraphs.Find(BoundGraph) == INDEX_NONE)
	{
		ParentGraph->SubGraphs.Add(BoundGraph);
	}
}
#endif

void URuleTransitionNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FAnimPhysObjectVersion::GUID);
	Ar.UsingCustomVersion(FUE5MainStreamObjectVersion::GUID);
}

void URuleTransitionNode::DestroyNode()
{
#if false
	// BoundGraph may be shared with another graph, if so, don't remove it here
	UEdGraph* GraphToRemove = IsBoundGraphShared() ? NULL : GetBoundGraph();

	BoundGraph = NULL;
#endif
	Super::DestroyNode();

#if false
	if (GraphToRemove)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
		FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
	}
#endif
}

#if false
/** Returns true if this nodes BoundGraph is shared with another node in the parent graph */
bool URuleTransitionNode::IsBoundGraphShared() const
{
	if (BoundGraph)
	{
		if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(GetGraph()))
		{
			TArray<URuleNodeBase*> StateNodes;
			FBlueprintEditorUtils::GetAllNodesOfClassEx<URuleNodeBase>(Blueprint, StateNodes);

			for (int32 NodeIdx = 0; NodeIdx < StateNodes.Num(); NodeIdx++)
			{
				URuleNodeBase* AnimNode = Cast<URuleNodeBase>(StateNodes[NodeIdx]);
				if ((AnimNode != NULL) && (AnimNode != this) && (AnimNode->GetBoundGraph() == BoundGraph))
				{
					return true;
				}
			}
		}
	}

	return false;
}
#endif

#if false
void URuleTransitionNode::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
	if (UAnimationTransitionGraph* TransGraph = Cast<UAnimationTransitionGraph>(BoundGraph))
	{
		UAnimGraphNode_TransitionResult* ResultNode = TransGraph->GetResultNode();
		check(ResultNode);

		if (bAutomaticRuleBasedOnSequencePlayerInState)
		{
			// Check for automatic transition rules that are being triggered from looping asset players, as these can often be symptomatic of logic errors
			if (URuleNodeBase* PreviousState = GetPreviousState())
			{
				if (UEdGraph* PreviousStateGraph = PreviousState->GetBoundGraph())
				{
					if (UAnimGraphNode_StateResult* PreviousStateGraphResultNode = Cast<UAnimationStateGraph>(PreviousStateGraph)->GetResultNode())
					{
						for (UEdGraphPin* TestPin : PreviousStateGraphResultNode->Pins)
						{
							// Warn for the trivial but common case of a looping asset player connected directly to the result node
							if ((TestPin->Direction == EGPD_Input) && (TestPin->LinkedTo.Num() == 1))
							{
								if (UAnimGraphNode_SequencePlayer* SequencePlayer = Cast<UAnimGraphNode_SequencePlayer>(TestPin->LinkedTo[0]->GetOwningNode()))
								{
									if (SequencePlayer->Node.IsLooping())
									{
										// MessageLog.Warning(TEXT("Transition @@ is using an automatic transition rule but the source @@ is set as looping.  Please clear the 'Loop Animation' flag"), this, SequencePlayer);
									}
								}
								else if (UAnimGraphNode_BlendSpacePlayer* BlendSpacePlayer = Cast<UAnimGraphNode_BlendSpacePlayer>(TestPin->LinkedTo[0]->GetOwningNode()))
								{
									if (BlendSpacePlayer->Node.IsLooping())
									{
										// MessageLog.Warning(TEXT("Transition @@ is using an automatic transition rule but the source @@ is set as looping.  Please clear the 'Loop' flag"), this, BlendSpacePlayer);
									}
								}
							}
						}
					}
				}
			}
		}
		else if (ResultNode->PropertyBindings.Num() > 0 && ResultNode->PropertyBindings.CreateIterator()->Value.bIsBound)
		{
			// Rule is bound so nothing more to check
		}
		else if (ResultNode->Pins.Num() > 0)
		{
			UEdGraphPin* BoolResultPin = ResultNode->Pins[0];
			if (BoolResultPin && (BoolResultPin->LinkedTo.Num() == 0) && (BoolResultPin->DefaultValue.ToBool() == false))
			{
				// check for native transition rule before warning
				bool bHasNative = false;
				UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
				if(Blueprint && Blueprint->ParentClass)
				{
					UAnimInstance* AnimInstance = CastChecked<UAnimInstance>(Blueprint->ParentClass->GetDefaultObject());
					if(AnimInstance)
					{
						UEdGraph* ParentGraph = GetGraph();
						URuleNodeBase* PrevState = GetPreviousState();
						URuleNodeBase* NextState = GetNextState();
						if(PrevState != nullptr && NextState != nullptr && ParentGraph != nullptr)
						{
							FName FunctionName;
							bHasNative = AnimInstance->HasNativeTransitionBinding(ParentGraph->GetFName(), FName(*PrevState->GetStateName()), FName(*NextState->GetStateName()), FunctionName);
						}
					}
				}

				if (!bHasNative && !bAutomaticRuleBasedOnSequencePlayerInState)
				{
					MessageLog.Warning(TEXT("@@ will never be taken, please connect something to @@"), this, BoolResultPin);
				}
			}
		}
	}
	else
	{
		MessageLog.Error(TEXT("@@ contains an invalid or NULL BoundGraph.  Please delete and recreate the transition."), this);
	}
}

UObject* URuleTransitionNode::GetJumpTargetForDoubleClick() const
{
	// Our base class uses GetSubGraphs. Since we explicitly ignore a shared bound graph, use BoundGraph directly instead.
	return BoundGraph;
}

TArray<UEdGraph*> URuleTransitionNode::GetSubGraphs() const
{ 
	TArray<UEdGraph*> SubGraphs;
	if(!IsBoundGraphShared())
	{
		SubGraphs.Add(BoundGraph);
	}
	return SubGraphs; 
}
#endif

#if false
//////////////////////////////////////////////////////////////////////////
// IRuleTransitionNodeSharedDataHelper

void IRuleTransitionNodeSharedDataHelper::UpdateSharedData(URuleTransitionNode* Node, TSharedPtr<INameValidatorInterface> NameValidator)
{
	// get all other transition nodes
	TArray<URuleTransitionNode*> TransitionNodes;

	UEdGraph* ParentGraph = Node->GetGraph();
	ParentGraph->GetNodesOfClass(TransitionNodes);

	// check if there is other node that can provide us with data
	for (TArray<URuleTransitionNode*>::TIterator It(TransitionNodes); It; ++ It)
	{
		URuleTransitionNode* OtherNode = *It;
		if (OtherNode != Node &&
			CheckIfHasDataToShare(OtherNode) &&
			CheckIfNodesShouldShareData(Node, OtherNode))
		{
			// use shared data of that node (to make sure everything is linked up properly)
			ShareData(Node, OtherNode);
			break;
		}
	}

	// check if our shared rule name is original
	if (NameValidator->FindValidString(AccessShareDataName(Node)) != EValidatorResult::Ok)
	{
		// rename all shared rules name in nodes that should share same name
		for (TArray<URuleTransitionNode*>::TIterator It(TransitionNodes); It; ++ It)
		{
			URuleTransitionNode* OtherNode = *It;
			if (OtherNode != Node &&
				CheckIfNodesShouldShareData(Node, OtherNode))
			{
				AccessShareDataName(OtherNode) = AccessShareDataName(Node);
			}
		}
	}
}

void IRuleTransitionNodeSharedDataHelper::MakeSureGuidExists(URuleTransitionNode* Node)
{
	UEdGraph* CurrentGraph = Node->GetGraph();
	for (int32 idx=0; idx < CurrentGraph->Nodes.Num(); idx++)
	{
		if (URuleTransitionNode* OtherNode = Cast<URuleTransitionNode>(CurrentGraph->Nodes[idx]))
		{
			if (OtherNode != Node &&
				CheckIfNodesShouldShareData(Node, OtherNode))
			{
				AccessShareDataName(Node) = AccessShareDataName(OtherNode);
			}
		}
	}
	
	if (! AccessShareDataGuid(Node).IsValid())
	{
		AccessShareDataGuid(Node) = FGuid::NewGuid();
	}
}

//////////////////////////////////////////////////////////////////////////
// FRuleTransitionNodeSharedRulesHelper

bool FRuleTransitionNodeSharedRulesHelper::CheckIfNodesShouldShareData(const URuleTransitionNode* NodeA, const URuleTransitionNode* NodeB)
{
	return NodeA->bSharedRules && NodeB->bSharedRules && NodeA->SharedRulesGuid == NodeB->SharedRulesGuid;
}

bool FRuleTransitionNodeSharedRulesHelper::CheckIfHasDataToShare(const URuleTransitionNode* Node)
{
	return Node->BoundGraph != NULL;
}

void FRuleTransitionNodeSharedRulesHelper::ShareData(URuleTransitionNode* NodeWhoWantsToShare, const URuleTransitionNode* ShareFrom)
{
	NodeWhoWantsToShare->UseSharedRules(ShareFrom);
}

FString& FRuleTransitionNodeSharedRulesHelper::AccessShareDataName(URuleTransitionNode* Node)
{
	return Node->SharedRulesName;
}

FGuid& FRuleTransitionNodeSharedRulesHelper::AccessShareDataGuid(URuleTransitionNode* Node)
{
	return Node->SharedRulesGuid;
}
#endif

#undef LOCTEXT_NAMESPACE
