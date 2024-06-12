// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesNode_State.h"
#include "DungeonRulesNode_Transition.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRules.h"

#define LOCTEXT_NAMESPACE "DungeonRuleNode"

UDungeonRulesNode_State::UDungeonRulesNode_State()
	: Super()
{
	bCanRenameNode = true;
}

void UDungeonRulesNode_State::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, FName("In"));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, FName("Out"));
}

FText UDungeonRulesNode_State::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

const UClass* UDungeonRulesNode_State::GetInstanceClass() const
{
	return UDungeonRule::StaticClass();
}

UEdGraphPin* UDungeonRulesNode_State::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UDungeonRulesNode_State::GetOutputPin() const
{
	return Pins[1];
}

FString UDungeonRulesNode_State::GetStateName() const
{
	const UDungeonRule* Rule = GetNodeInstance<UDungeonRule>();
	return Rule ? Rule->RuleName : TEXT("NULL");
}

TArray<FName> UDungeonRulesNode_State::GetPropertyNamesToEdit() const
{
	return {GET_MEMBER_NAME_CHECKED(UDungeonRule, RoomChooser)};
}

#undef LOCTEXT_NAMESPACE
