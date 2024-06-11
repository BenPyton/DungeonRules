// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "SGraphNode.h"

class SGraphPin;
class URuleExitNode;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;

class SGraphNodeDungeonRuleExit : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRuleExit){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URuleExitNode* InNode);

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	//~ End SGraphNode Interface

protected:
	FSlateColor GetBorderBackgroundColor() const;
	FText GetPreviewCornerText() const;
};
