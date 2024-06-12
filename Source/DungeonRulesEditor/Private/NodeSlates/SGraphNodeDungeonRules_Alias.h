// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "SGraphNodeDungeonRules_State.h"

class SToolTip;
class UDungeonRulesNode_Alias;
class UEdGraphNode;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FSlateBrush;

class SGraphNodeDungeonRules_Alias : public SGraphNodeDungeonRules_State
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRules_Alias) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UDungeonRulesNode_Alias* InNode);

protected:
	//~ Begin SGraphNode Interface
	virtual bool IsNameReadOnly() const override;
	//~ End SGraphNode Interface

	virtual FSlateColor GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const;

	//~ Begin SGraphNodeDungeonRules_State Interface
	virtual FText GetPreviewCornerText() const override;
	virtual const FSlateBrush* GetNameIcon() const override;
	virtual FText GetNodeName() const override;
	//~ End SGraphNodeDungeonRules_State Interface
};
