// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Internationalization/Text.h"
#include "Math/Color.h"
#include "SGraphNodeDungeonRule.h"
#include "Styling/SlateColor.h"
#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SToolTip;
class URuleAliasNode;
class UEdGraphNode;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FSlateBrush;

class SGraphNodeDungeonRuleAlias : public SGraphNodeDungeonRule
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRuleAlias) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URuleAliasNode* InNode);

	// SNodePanel::SNode interface
#if false // Blueprint
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
#endif
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SNodePanel::SNode interface

#if false // Blueprint
	static void GetStateInfoPopup(UEdGraphNode* GraphNode, TArray<FGraphInformationPopupInfo>& Popups);
#endif

protected:
	virtual FSlateColor GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const;

	virtual FText GetPreviewCornerText() const override;
	virtual const FSlateBrush* GetNameIcon() const override;
};
