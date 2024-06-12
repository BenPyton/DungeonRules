// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesNode_Alias.h"
#include "DungeonRulesNode_State.h"
#include "DungeonRulesEdTypes.h"

class FArchive;

#define LOCTEXT_NAMESPACE "DungeonRulesNode_Alias"

/////////////////////////////////////////////////////
// UDungeonRulesNode_Alias

UDungeonRulesNode_Alias::UDungeonRulesNode_Alias()
	: Super()
{
	bCanRenameNode = true;
	StateAliasName = GetDesiredNewNodeName();
}

void UDungeonRulesNode_Alias::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	RebuildAliasedStateNodeReferences();
}

void UDungeonRulesNode_Alias::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
}

FText UDungeonRulesNode_Alias::GetTooltipText() const
{
	if (bGlobalAlias)
		return LOCTEXT("AliasNodeTooltip_AnyState", "Alias for any other state.");

	if (const UDungeonRulesNode* AliasedState = GetAliasedState())
		return FText::Format(LOCTEXT("AliasNodeTooltip_OneState", "Alias for {0}."), FText::FromString(AliasedState->GetStateName()));

	if (AliasedStateNodes.Num() <= 0)
		return LOCTEXT("AliasNodeTooltip_NoState", "No state aliased.");

	FString StateNames;
	for (const auto& AliasedState : AliasedStateNodes)
	{
		StateNames += TEXT("\n- ") + AliasedState->GetStateName();
	}

	return FText::Format(LOCTEXT("AliasNodeTooltip_MultiState", "Alias for states:{0}"), FText::FromString(StateNames));
}

FString UDungeonRulesNode_Alias::GetStateName() const
{
	return StateAliasName;
}

void UDungeonRulesNode_Alias::OnRenameNode(const FString& NewName)
{
	StateAliasName = NewName;
}

UEdGraphPin* UDungeonRulesNode_Alias::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UDungeonRulesNode_Alias::GetOutputPin() const
{
	return Pins[1];
}

FString UDungeonRulesNode_Alias::GetDesiredNewNodeName() const
{
	return TEXT("Alias");
}

UObject* UDungeonRulesNode_Alias::GetJumpTargetForDoubleClick() const
{
	return GetAliasedState();
}

const TSet<TWeakObjectPtr<UDungeonRulesNode>>& UDungeonRulesNode_Alias::GetAliasedStates() const
{
	return AliasedStateNodes;
}

TSet<TWeakObjectPtr<UDungeonRulesNode>>& UDungeonRulesNode_Alias::GetAliasedStates()
{
	return AliasedStateNodes;
}

UDungeonRulesNode* UDungeonRulesNode_Alias::GetAliasedState() const
{
	// If we alias more than one state, we return null
	if (bGlobalAlias || (AliasedStateNodes.Num() != 1))
	{
		return nullptr;
	}


	if (UDungeonRulesNode* AliasedState = AliasedStateNodes.CreateConstIterator()->Get())
	{
		if (IsValidChecked(AliasedState))
		{
			if (const UEdGraph* Graph = GetGraph())
			{
				TArray<UDungeonRulesNode*> StateNodes;
				Graph->GetNodesOfClassEx<UDungeonRulesNode_State, UDungeonRulesNode>(StateNodes);
				return StateNodes.Contains(AliasedState) ? AliasedState : nullptr;
			}
		}
	}
	
	return nullptr;
}

void UDungeonRulesNode_Alias::RebuildAliasedStateNodeReferences()
{
	TSet<TWeakObjectPtr<UDungeonRulesNode>> NewAliasedStateNodes;

	// We don't use UEdGraphNode::GetGraph because this may be called during deletion and we don't want to assert on a missing graph.
	if (const UEdGraph* Graph = Cast<UEdGraph>(GetOuter()))
	{
		TArray<UDungeonRulesNode*> StateNodes;
		Graph->GetNodesOfClassEx<UDungeonRulesNode_State, UDungeonRulesNode>(StateNodes);
		TSet<UDungeonRulesNode*> StateNodesSet(StateNodes);

		for (const auto& StateNode : AliasedStateNodes)
		{
			// Keep only nodes that are still in the graph
			if (StateNode.IsValid() && StateNodesSet.Contains(StateNode.Get()))
			{
				NewAliasedStateNodes.Add(StateNode);
			}
		}

		AliasedStateNodes = NewAliasedStateNodes;
	}
}

#undef LOCTEXT_NAMESPACE
