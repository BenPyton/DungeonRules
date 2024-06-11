// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Internationalization/Text.h"
#include "Math/Color.h"
#include "SGraphNode.h"
#include "Styling/SlateColor.h"
#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SGraphPin;
class SToolTip;
class URuleConduitNode;
class URuleNodeBase;
class UEdGraphNode;
struct FGeometry;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FPointerEvent;
struct FSlateBrush;

class SGraphNodeDungeonRule : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRule){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URuleNodeBase* InNode);

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

#if false // Blueprint
	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	static void GetStateInfoPopup(UEdGraphNode* GraphNode, TArray<FGraphInformationPopupInfo>& Popups);
#endif

protected:
	FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const;

	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
	virtual FText GetNodeName() const;

private:
	TSharedPtr<SNodeTitle> NodeTitle {nullptr};
};
