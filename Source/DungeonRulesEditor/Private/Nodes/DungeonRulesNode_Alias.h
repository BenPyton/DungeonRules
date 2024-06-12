// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRulesNode.h"
#include "DungeonRulesNode_Alias.generated.h"

class FArchive;
class UDungeonRulesNode_State;
class UEdGraph;
class UEdGraphPin;
class UObject;

UCLASS(MinimalAPI)
class UDungeonRulesNode_Alias : public UDungeonRulesNode
{
	GENERATED_BODY()
public:
	UDungeonRulesNode_Alias();

	// This will alias any state. Takes into account new states created after this alias has been setup.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Alias")
	bool bGlobalAlias;

	UPROPERTY()
	FString StateAliasName;

	// UObject
	virtual void Serialize(FArchive& Ar) override;

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void OnRenameNode(const FString& NewName) override;
	//~ End UEdGraphNode Interface

	//~ Begin UDungeonRulesNode Interface
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	virtual FString GetDesiredNewNodeName() const override;
	UObject* GetJumpTargetForDoubleClick() const;
	//~ End UDungeonRulesNode Interface
	
	DUNGEONRULESEDITOR_API const TSet<TWeakObjectPtr<UDungeonRulesNode>>& GetAliasedStates() const;
	DUNGEONRULESEDITOR_API TSet<TWeakObjectPtr<UDungeonRulesNode>>& GetAliasedStates();

	// Returns null if aliasing more than one state.
	DUNGEONRULESEDITOR_API UDungeonRulesNode* GetAliasedState() const;

private:

	void RebuildAliasedStateNodeReferences();

	UPROPERTY()
	TSet<TWeakObjectPtr<UDungeonRulesNode>> AliasedStateNodes;
};
