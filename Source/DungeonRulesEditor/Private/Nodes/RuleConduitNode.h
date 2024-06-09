// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RuleNodeBase.h"
#include "RuleConduitNode.generated.h"

class UEdGraph;
class UEdGraphPin;

UCLASS(MinimalAPI)
class URuleConduitNode : public URuleNodeBase
{
	GENERATED_BODY()
public:
	URuleConduitNode();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	//~ End UEdGraphNode Interface

	//~ Begin UDungeonRuleNodeBase Interface
	virtual const UClass* GetInstanceClass() const override;
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	virtual FString GetDesiredNewNodeName() const override;
	//~ End UDungeonRuleNodeBase Interface
};
