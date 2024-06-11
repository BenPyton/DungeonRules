// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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

FText URuleNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

const UClass* URuleNode::GetInstanceClass() const
{
	return UDungeonRule::StaticClass();
}

UEdGraphPin* URuleNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleNode::GetOutputPin() const
{
	return Pins[1];
}

FString URuleNode::GetStateName() const
{
	const UDungeonRule* Rule = GetNodeInstance<UDungeonRule>();
	return Rule ? Rule->RuleName : TEXT("NULL");
}

TArray<FName> URuleNode::GetPropertyNamesToEdit() const
{
	return {GET_MEMBER_NAME_CHECKED(UDungeonRule, RoomChooser)};
}

#undef LOCTEXT_NAMESPACE
