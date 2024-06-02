// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	DungeonRuleNode.cpp
=============================================================================*/

#include "RuleExitNode.h"

#include "DungeonRulesSchema.h"
#include "Containers/Array.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
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
