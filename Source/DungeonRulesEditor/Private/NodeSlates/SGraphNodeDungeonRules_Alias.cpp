// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "SGraphNodeDungeonRules_Alias.h"
#include "Nodes/DungeonRulesNode_Alias.h"

class SToolTip;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRules_Alias"

/////////////////////////////////////////////////////
// SGraphNodeDungeonRules_Alias

void SGraphNodeDungeonRules_Alias::Construct(const FArguments& InArgs, UDungeonRulesNode_Alias* InNode)
{
	SGraphNodeDungeonRules_State::Construct(SGraphNodeDungeonRules_State::FArguments(), InNode);
}

bool SGraphNodeDungeonRules_Alias::IsNameReadOnly() const
{
	if (UDungeonRulesNode_Alias* AliasNode = Cast<UDungeonRulesNode_Alias>(GraphNode))
	{
		if (AliasNode->bGlobalAlias
			|| AliasNode->GetAliasedState()
			|| AliasNode->GetAliasedStates().Num() <= 0)
			return true;
	}

	return SGraphNode::IsNameReadOnly();
}

FSlateColor SGraphNodeDungeonRules_Alias::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
	// Override inactive state color for aliases.
	return FLinearColor(0.45f, 0.33f, 0.37f);
}

FText SGraphNodeDungeonRules_Alias::GetPreviewCornerText() const
{
	return FText();
}

const FSlateBrush* SGraphNodeDungeonRules_Alias::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.AliasNode.Icon"));
}

FText SGraphNodeDungeonRules_Alias::GetNodeName() const
{
	if (UDungeonRulesNode_Alias* AliasNode = Cast<UDungeonRulesNode_Alias>(GraphNode))
	{
		if (AliasNode->bGlobalAlias)
			return FText::FromString(TEXT("Any"));

		if (UDungeonRulesNode* AliasedNode = AliasNode->GetAliasedState())
			return FText::FromString(FString::Printf(TEXT("@%s"), *AliasedNode->GetStateName()));

		if (AliasNode->GetAliasedStates().Num() <= 0)
			return FText::FromString(TEXT("None"));
	}

	return SGraphNodeDungeonRules_State::GetNodeName();
}

#undef LOCTEXT_NAMESPACE
