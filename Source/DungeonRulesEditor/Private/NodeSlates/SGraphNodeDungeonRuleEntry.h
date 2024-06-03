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

	//~ Begin SGraphNode Interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	//~ End SGraphNode Interface

protected:
	FSlateColor GetBorderBackgroundColor() const;
	FText GetPreviewCornerText() const;
};
