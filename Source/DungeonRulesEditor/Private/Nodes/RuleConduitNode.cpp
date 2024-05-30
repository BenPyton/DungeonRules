// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleConduitNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/CompilerResultsLog.h"
//#include "AnimationTransitionGraph.h"
//#include "AnimationConduitGraphSchema.h"
//#include "AnimGraphNode_TransitionResult.h"

#define LOCTEXT_NAMESPACE "RuleConduitNode"

/////////////////////////////////////////////////////
// URuleConduitNode

URuleConduitNode::URuleConduitNode()
	: Super()
{
	bCanRenameNode = true;
}

void URuleConduitNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

void URuleConduitNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FText URuleConduitNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

FText URuleConduitNode::GetTooltipText() const
{
	return LOCTEXT("ConduitNodeTooltip", "This is a conduit, which allows specification of a predicate condition for an entire group of transitions");
}

FString URuleConduitNode::GetStateName() const
{
	return (BoundGraph != NULL) ? *(BoundGraph->GetName()) : TEXT("(null)");
}

UEdGraphPin* URuleConduitNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleConduitNode::GetOutputPin() const
{
	return Pins[1];
}

void URuleConduitNode::PostPlacedNewNode()
{
#if false
	// Create a new animation graph
	check(BoundGraph == NULL);
	BoundGraph = FBlueprintEditorUtils::CreateNewGraph(
		this,
		NAME_None,
		UAnimationTransitionGraph::StaticClass(),
		UAnimationConduitGraphSchema::StaticClass());
	check(BoundGraph);

	// Find an interesting name
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, TEXT("Conduit"));

	// Initialize the transition graph
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

void URuleConduitNode::DestroyNode()
{
	UEdGraph* GraphToRemove = BoundGraph;

	BoundGraph = NULL;
	Super::DestroyNode();

	if (GraphToRemove)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
		FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
	}
}

void URuleConduitNode::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
#if false
	UAnimationTransitionGraph* TransGraph = CastChecked<UAnimationTransitionGraph>(BoundGraph);
	UAnimGraphNode_TransitionResult* ResultNode = TransGraph->GetResultNode();
	check(ResultNode);

	if (ResultNode->PropertyBindings.Num() > 0 && ResultNode->PropertyBindings.CreateIterator()->Value.bIsBound)
	{
		// Rule is bound so nothing more to check
	}
	else
	{
		UEdGraphPin* BoolResultPin = ResultNode->Pins[0];
		if ((BoolResultPin->LinkedTo.Num() == 0) && (BoolResultPin->DefaultValue.ToBool() == false))
		{
			MessageLog.Warning(TEXT("@@ will never be taken, please connect something to @@"), this, BoolResultPin);
		}
	}
#endif
}

FString URuleConduitNode::GetDesiredNewNodeName() const
{
	return TEXT("Conduit");
}

void URuleConduitNode::PostPasteNode()
{
	// Find an interesting name, but try to keep the same if possible
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, GetStateName());
	Super::PostPasteNode();
}

#undef LOCTEXT_NAMESPACE
