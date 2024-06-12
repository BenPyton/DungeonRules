// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

/*=============================================================================
	DungeonRuleNode.cpp
=============================================================================*/

#include "DungeonRulesNode_Begin.h"
#include "DungeonRulesSchema.h"
#include "DungeonRulesEdTypes.h"

#define LOCTEXT_NAMESPACE "DungeonRulesNode_Begin"

/////////////////////////////////////////////////////
// UDungeonRulesNode_Begin

UDungeonRulesNode_Begin::UDungeonRulesNode_Begin(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDungeonRulesNode_Begin::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Exec, TEXT("Begin"));
}

FText UDungeonRulesNode_Begin::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText UDungeonRulesNode_Begin::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UDungeonRulesNode_Begin::GetOutputNode() const
{
	if(Pins.Num() > 0 && Pins[0] != NULL)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if(Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return NULL;
}

#undef LOCTEXT_NAMESPACE
