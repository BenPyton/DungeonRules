// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Internationalization/Text.h"
#include "SGraphNode.h"
#include "Styling/SlateColor.h"
#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SGraphPin;
class URuleEntryNode;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;

class SGraphNodeDungeonRuleEntry : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeDungeonRuleEntry){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URuleEntryNode* InNode);

#if false // Blueprint
	//~ Begin SNodePanel::SNode Interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	//~ End SNodePanel::SNode Interface
#endif

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	//~ End SGraphNode Interface

protected:
	FSlateColor GetBorderBackgroundColor() const;
	FText GetPreviewCornerText() const;
};
