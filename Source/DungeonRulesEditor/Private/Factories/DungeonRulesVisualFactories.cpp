// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesVisualFactories.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRulesSchema.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleConduitNode.h"
#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleExitNode.h"
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
#include "NodeSlates/SGraphNodeDungeonRule.h"
#include "NodeSlates/SGraphNodeDungeonRuleAlias.h"
#include "NodeSlates/SGraphNodeDungeonRuleConduit.h"
#include "NodeSlates/SGraphNodeDungeonRuleEntry.h"
#include "NodeSlates/SGraphNodeDungeonRuleExit.h"
#include "NodeSlates/SGraphNodeDungeonRuleTransition.h"
#include "DungeonRulesConnectionDrawingPolicy.h"
#include "KismetPins/SGraphPinExec.h"

TSharedPtr<class SGraphNode> FDungeonRulesNodeFactory::CreateNode(class UEdGraphNode* InNode) const 
{
	if (URuleTransitionNode* TransitionNode = Cast<URuleTransitionNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleTransition, TransitionNode);
	}
	else if (URuleNode* StateNode = Cast<URuleNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRule, StateNode);
	}
	else if (URuleAliasNode* StateAliasNode = Cast<URuleAliasNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleAlias, StateAliasNode);
	}
	else if (URuleConduitNode* ConduitNode = Cast<URuleConduitNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleConduit, ConduitNode);
	}
	else if (URuleEntryNode* EntryNode = Cast<URuleEntryNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleEntry, EntryNode);
	}
	else if (URuleExitNode* ExitNode = Cast<URuleExitNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleExit, ExitNode);
	}
	
	return nullptr;
}

TSharedPtr<class SGraphPin> FDungeonRulesPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->GetSchema()->IsA<UDungeonRulesSchema>() && InPin->PinType.PinCategory == DungeonRulesPinCategory::Exec)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}

class FConnectionDrawingPolicy* FDungeonRulesPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	if (Schema->IsA(UDungeonRulesSchema::StaticClass()))
	{
		return new FDungeonRulesConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}

	return nullptr;
}
