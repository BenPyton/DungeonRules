// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRulesNode.h"
#include "DungeonRulesNode_Conduit.generated.h"

class UEdGraph;
class UEdGraphPin;

UCLASS(MinimalAPI)
class UDungeonRulesNode_Conduit : public UDungeonRulesNode
{
	GENERATED_BODY()
public:
	UDungeonRulesNode_Conduit();

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
