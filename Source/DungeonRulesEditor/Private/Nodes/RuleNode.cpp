// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "DungeonRulesGraph.h"
#include "Kismet2/Kismet2NameValidators.h"
//#include "AnimGraphNode_StateResult.h"
//#include "DungeonRulesGraphSchema.h"
#include "DungeonRulesEdTypes.h"

#define LOCTEXT_NAMESPACE "DungeonRuleNode"

URuleNode::URuleNode()
	: Super()
	, NodeName(TEXT("None"))
{
	bCanRenameNode = true;
}

void URuleNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, FName("In"));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, FName("Out"));
}

void URuleNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	//@TODO: If the FromPin is a state, create a transition between us
	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FText URuleNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

FText URuleNode::GetTooltipText() const
{
	return LOCTEXT("DungeonRuleNode_Tooltip", "This is a state");
}

FString URuleNode::GetStateName() const
{
#if false
	return (BoundGraph != NULL) ? *(BoundGraph->GetName()) : TEXT("(null)");
#else
	return NodeName;
#endif
}


UEdGraphPin* URuleNode::GetInputPin() const
{
	return Pins[0];
}


UEdGraphPin* URuleNode::GetOutputPin() const
{
	return Pins[1];
}

void URuleNode::PostPasteNode()
{
#if false
	// Find an interesting name, but try to keep the same if possible
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, GetStateName());

	for (UEdGraphNode* GraphNode : BoundGraph->Nodes)
	{
		GraphNode->CreateNewGuid();
		GraphNode->PostPasteNode();
		GraphNode->ReconstructNode();
	}
#endif
	Super::PostPasteNode();
}

void URuleNode::PostPlacedNewNode()
{
#if false
	// Create a new animation graph
	check(BoundGraph == NULL);
	BoundGraph = FBlueprintEditorUtils::CreateNewGraph(
		this,
		NAME_None,
		UAnimationStateGraph::StaticClass(),
		UAnimationStateGraphSchema::StaticClass());
	check(BoundGraph);

	// Find an interesting name
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, TEXT("State"));

	// Initialize the anim graph
	const UEdGraphSchema* Schema = BoundGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*BoundGraph);

	// Add the new graph as a child of our parent graph
	UEdGraph* ParentGraph = GetGraph();

	if(ParentGraph->SubGraphs.Find(BoundGraph) == INDEX_NONE)
	{
		ParentGraph->SubGraphs.Add(BoundGraph);
	}
#endif
}

void URuleNode::DestroyNode()
{
#if false
	UEdGraph* GraphToRemove = BoundGraph;

	BoundGraph = NULL;
	Super::DestroyNode();

	if (GraphToRemove)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
		FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
	}
#else
	Super::DestroyNode();
#endif
}

void URuleNode::OnRenameNode(const FString& NewName)
{
	NodeName = NewName;
}

#undef LOCTEXT_NAMESPACE
