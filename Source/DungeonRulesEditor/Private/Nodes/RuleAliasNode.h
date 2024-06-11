// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "RuleNodeBase.h"
#include "Animation/AnimTypes.h"
#include "Containers/Set.h"
#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Internationalization/Text.h"
#include "Misc/AssertionMacros.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "RuleAliasNode.generated.h"

class FArchive;
class URuleNode;
class UEdGraph;
class UEdGraphPin;
class UObject;

UCLASS(MinimalAPI)
class URuleAliasNode : public URuleNodeBase
{
	GENERATED_BODY()
public:
	URuleAliasNode();

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
#if false // Blueprint
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
#endif
	//~ End UEdGraphNode Interface

	//~ Begin URuleNodeBase Interface
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	virtual FString GetDesiredNewNodeName() const override;
	UObject* GetJumpTargetForDoubleClick() const;
	//~ End URuleNodeBase Interface
	
	DUNGEONRULESEDITOR_API const TSet<TWeakObjectPtr<URuleNodeBase>>& GetAliasedStates() const;
	DUNGEONRULESEDITOR_API TSet<TWeakObjectPtr<URuleNodeBase>>& GetAliasedStates();

	// Returns null if aliasing more than one state.
	DUNGEONRULESEDITOR_API URuleNodeBase* GetAliasedState() const;

private:

	void RebuildAliasedStateNodeReferences();

	UPROPERTY()
	TSet<TWeakObjectPtr<URuleNodeBase>> AliasedStateNodes;
};
