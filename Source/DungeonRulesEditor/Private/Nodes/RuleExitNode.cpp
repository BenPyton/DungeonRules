// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

/*=============================================================================
	DungeonRuleNode.cpp
=============================================================================*/

#include "RuleExitNode.h"
#include "DungeonRulesSchema.h"
#include "DungeonRulesEdTypes.h"

#define LOCTEXT_NAMESPACE "RuleExitNode"

/////////////////////////////////////////////////////
// URuleExitNode

URuleExitNode::URuleExitNode()
	: Super()
{
}

void URuleExitNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Exec, TEXT("Exit"));
}

FText URuleExitNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText URuleExitNode::GetTooltipText() const
{
	return LOCTEXT("DungeonRuleExitNodeTooltip", "Exit point for state machine");
}

#undef LOCTEXT_NAMESPACE
