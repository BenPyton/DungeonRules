// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesVisualFactories.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRulesSchema.h"
#include "Nodes/DungeonRulesNode_Alias.h"
#include "Nodes/DungeonRulesNode_Begin.h"
#include "Nodes/DungeonRulesNode_Conduit.h"
#include "Nodes/DungeonRulesNode_State.h"
#include "Nodes/DungeonRulesNode_Stop.h"
#include "Nodes/DungeonRulesNode_Transition.h"
#include "NodeSlates/SGraphNodeDungeonRules_Alias.h"
#include "NodeSlates/SGraphNodeDungeonRules_Begin.h"
#include "NodeSlates/SGraphNodeDungeonRules_Conduit.h"
#include "NodeSlates/SGraphNodeDungeonRules_State.h"
#include "NodeSlates/SGraphNodeDungeonRules_Stop.h"
#include "NodeSlates/SGraphNodeDungeonRules_Transition.h"
#include "DungeonRulesConnectionDrawingPolicy.h"
#include "KismetPins/SGraphPinExec.h"

TSharedPtr<class SGraphNode> FDungeonRulesNodeFactory::CreateNode(class UEdGraphNode* InNode) const 
{
	if (UDungeonRulesNode_Transition* TransitionNode = Cast<UDungeonRulesNode_Transition>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_Transition, TransitionNode);
	}
	else if (UDungeonRulesNode_State* StateNode = Cast<UDungeonRulesNode_State>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_State, StateNode);
	}
	else if (UDungeonRulesNode_Alias* StateAliasNode = Cast<UDungeonRulesNode_Alias>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_Alias, StateAliasNode);
	}
	else if (UDungeonRulesNode_Conduit* ConduitNode = Cast<UDungeonRulesNode_Conduit>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_Conduit, ConduitNode);
	}
	else if (UDungeonRulesNode_Begin* EntryNode = Cast<UDungeonRulesNode_Begin>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_Begin, EntryNode);
	}
	else if (UDungeonRulesNode_Stop* ExitNode = Cast<UDungeonRulesNode_Stop>(InNode))
	{
		return SNew(SGraphNodeDungeonRules_Stop, ExitNode);
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
