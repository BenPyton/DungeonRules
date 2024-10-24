// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRulesNode.h"
#include "DungeonRulesNode_State.generated.h"

class UEdGraph;
class UEdGraphPin;
class UDungeonRule;

UCLASS(MinimalAPI)
class UDungeonRulesNode_State : public UDungeonRulesNode
{
	GENERATED_BODY()
public:
	UDungeonRulesNode_State();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanDuplicateNode() const override { return true; }
	//~ End UEdGraphNode Interface
	
	//~ Begin UDungeonRulesNode Interface
	virtual const UClass* GetInstanceClass() const override;
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	virtual FString GetDesiredNewNodeName() const { return TEXT("New Rule"); }
	virtual TArray<FName> GetPropertyNamesToEdit() const override;
	//~ End UDungeonRulesNode Interface
};
