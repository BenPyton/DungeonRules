// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

/*=============================================================================
	DungeonRuleNode.cpp
=============================================================================*/

#include "DungeonRulesNode_Stop.h"
#include "DungeonRulesSchema.h"
#include "DungeonRulesEdTypes.h"

#define LOCTEXT_NAMESPACE "DungeonRulesNode_Stop"

/////////////////////////////////////////////////////
// UDungeonRulesNode_Stop

UDungeonRulesNode_Stop::UDungeonRulesNode_Stop()
	: Super()
{
}

void UDungeonRulesNode_Stop::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Exec, TEXT("Stop"));
}

FText UDungeonRulesNode_Stop::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText UDungeonRulesNode_Stop::GetTooltipText() const
{
	return LOCTEXT("DungeonRuleExitNodeTooltip", "Exit point for state machine");
}

#undef LOCTEXT_NAMESPACE
