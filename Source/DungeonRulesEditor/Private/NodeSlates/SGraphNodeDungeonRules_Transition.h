// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "SGraphNode.h"

class STextEntryPopup;
class SToolTip;
class SWidget;
class UDungeonRulesNode_Transition;
class UObject;
struct FGeometry;
struct FPointerEvent;
struct FSlateBrush;

class SGraphNodeDungeonRules_Transition : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRules_Transition){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UDungeonRulesNode_Transition* InNode);

	//~ Begin SNodePanel::SNode Interface
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;
	//~ End SNodePanel::SNode Interface

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	//~ End SGraphNode Interface

	// SWidget interface
	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	static FLinearColor StaticGetTransitionColor(UDungeonRulesNode_Transition* TransNode, bool bIsHovered);

private:
	TSharedPtr<STextEntryPopup> TextEntryWidget;

	/** Cache of the widget representing the previous state node */
	mutable TWeakPtr<SNode> PrevStateNodeWidgetPtr;

private:
	FText GetPreviewCornerText(bool reverse) const;
	FSlateColor GetTransitionColor() const;
	const FSlateBrush* GetTransitionIconImage() const;
	FText GetTransitionPriorityOrder() const;

	TSharedRef<SWidget> GenerateRichTooltip();
};
