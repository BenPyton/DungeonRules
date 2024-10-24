// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "SGraphNode.h"

class SGraphPin;
class SToolTip;
class UDungeonRulesNode_Conduit;
class UDungeonRulesNode;
class UEdGraphNode;
struct FGeometry;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FPointerEvent;
struct FSlateBrush;

class SGraphNodeDungeonRules_State : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRules_State){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UDungeonRulesNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface

	// SWidget interface
	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

protected:
	FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const;

	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
	virtual FText GetNodeName() const;

private:
	TSharedPtr<SNodeTitle> NodeTitle {nullptr};
};
