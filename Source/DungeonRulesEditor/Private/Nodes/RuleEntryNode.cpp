// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	DungeonRuleNode.cpp
=============================================================================*/

#include "RuleEntryNode.h"

#include "DungeonRulesSchema.h"
#include "Containers/Array.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
#include "DungeonRulesEdTypes.h"

#define LOCTEXT_NAMESPACE "RuleEntryNode"

/////////////////////////////////////////////////////
// URuleEntryNode

URuleEntryNode::URuleEntryNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URuleEntryNode::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Exec, TEXT("Entry"));
}

FText URuleEntryNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText URuleEntryNode::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* URuleEntryNode::GetOutputNode() const
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
