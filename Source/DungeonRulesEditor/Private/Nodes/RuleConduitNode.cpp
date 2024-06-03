// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleConduitNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/CompilerResultsLog.h"
#include "DungeonRulesEdTypes.h"

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
	CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
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

UEdGraphPin* URuleConduitNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleConduitNode::GetOutputPin() const
{
	return Pins[1];
}

FString URuleConduitNode::GetStateName() const
{
	return TEXT("(null)");
}

FString URuleConduitNode::GetDesiredNewNodeName() const
{
	return TEXT("Conduit");
}

#undef LOCTEXT_NAMESPACE
