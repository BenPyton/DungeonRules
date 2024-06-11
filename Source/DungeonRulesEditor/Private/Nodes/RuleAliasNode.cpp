// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "RuleAliasNode.h"

#include "RuleNode.h"
#include "Containers/Array.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Templates/Casts.h"
#include "Templates/ChooseClass.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"
#include "DungeonRulesEdTypes.h"

class FArchive;

#define LOCTEXT_NAMESPACE "RuleAliasNode"

/////////////////////////////////////////////////////
// URuleAliasNode

URuleAliasNode::URuleAliasNode()
	: Super()
{
	bCanRenameNode = true;
	StateAliasName = GetDesiredNewNodeName();
}

void URuleAliasNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	RebuildAliasedStateNodeReferences();
}

void URuleAliasNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
}

#if false // Blueprint
void URuleAliasNode::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if ((GetInputPin()->LinkedTo.Num() > 0) && (bGlobalAlias || AliasedStateNodes.Num() != 1))
	{
		MessageLog.Error(*LOCTEXT("AliasAsEntryState", "A alias (@@) used as a transition's target must alias a single state").ToString(), this);
	}
}
#endif

FText URuleAliasNode::GetTooltipText() const
{
	if (bGlobalAlias)
		return LOCTEXT("AliasNodeTooltip_AnyState", "Alias for any other state.");

	if (const URuleNodeBase* AliasedState = GetAliasedState())
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

FString URuleAliasNode::GetStateName() const
{
	return StateAliasName;
}

void URuleAliasNode::OnRenameNode(const FString& NewName)
{
	StateAliasName = NewName;
}

UEdGraphPin* URuleAliasNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleAliasNode::GetOutputPin() const
{
	return Pins[1];
}

FString URuleAliasNode::GetDesiredNewNodeName() const
{
	return TEXT("Alias");
}

UObject* URuleAliasNode::GetJumpTargetForDoubleClick() const
{
	return GetAliasedState();
}

const TSet<TWeakObjectPtr<URuleNodeBase>>& URuleAliasNode::GetAliasedStates() const
{
	return AliasedStateNodes;
}

TSet<TWeakObjectPtr<URuleNodeBase>>& URuleAliasNode::GetAliasedStates()
{
	return AliasedStateNodes;
}

URuleNodeBase* URuleAliasNode::GetAliasedState() const
{
	// If we alias more than one state, we return null
	if (bGlobalAlias || (AliasedStateNodes.Num() != 1))
	{
		return nullptr;
	}


	if (URuleNodeBase* AliasedState = AliasedStateNodes.CreateConstIterator()->Get())
	{
		if (IsValidChecked(AliasedState))
		{
			if (const UEdGraph* Graph = GetGraph())
			{
				TArray<URuleNodeBase*> StateNodes;
				Graph->GetNodesOfClassEx<URuleNode, URuleNodeBase>(StateNodes);
				return StateNodes.Contains(AliasedState) ? AliasedState : nullptr;
			}
		}
	}
	
	return nullptr;
}

void URuleAliasNode::RebuildAliasedStateNodeReferences()
{
	TSet<TWeakObjectPtr<URuleNodeBase>> NewAliasedStateNodes;

	// We don't use UEdGraphNode::GetGraph because this may be called during deletion and we don't want to assert on a missing graph.
	if (const UEdGraph* Graph = Cast<UEdGraph>(GetOuter()))
	{
		TArray<URuleNodeBase*> StateNodes;
		Graph->GetNodesOfClassEx<URuleNode, URuleNodeBase>(StateNodes);
		TSet<URuleNodeBase*> StateNodesSet(StateNodes);

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
