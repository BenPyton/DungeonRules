// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RuleNodeBase.h"
#include "Internationalization/Text.h"
#include "UObject/UObjectGlobals.h"
#include "RuleExitNode.generated.h"

class UObject;

UCLASS(MinimalAPI)
class URuleExitNode : public URuleNodeBase
{
	GENERATED_BODY()

public:
	URuleExitNode();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override { return false; }
	//~ End UEdGraphNode Interface
	
	//~ Begin URuleNodeBase Interface
	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual FString GetStateName() const override { return TEXT("Exit"); }
	//~ End URuleNodeBase Interface
};
