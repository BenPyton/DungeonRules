// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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

protected:
	//~ Begin SGraphNode Interface
	virtual bool IsNameReadOnly() const override;
	//~ End SGraphNode Interface

	virtual FSlateColor GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const;

	//~ Begin SGraphNodeDungeonRule Interface
	virtual FText GetPreviewCornerText() const override;
	virtual const FSlateBrush* GetNameIcon() const override;
	virtual FText GetNodeName() const override;
	//~ End SGraphNodeDungeonRule Interface
};
