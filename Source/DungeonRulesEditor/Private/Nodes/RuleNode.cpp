// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "DungeonRulesGraph.h"
#include "DungeonRules.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRulesEdLog.h"

#define LOCTEXT_NAMESPACE "DungeonRuleNode"

URuleNode::URuleNode()
	: Super()
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

const UClass* URuleNode::GetInstanceClass() const
{
	return UDungeonRule::StaticClass();
}

FString URuleNode::GetStateName() const
{
	const UDungeonRule* Rule = GetNodeInstance<UDungeonRule>();
	return Rule ? Rule->RuleName : TEXT("NULL");
}

UEdGraphPin* URuleNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleNode::GetOutputPin() const
{
	return Pins[1];
}

#undef LOCTEXT_NAMESPACE
