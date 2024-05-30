// Copyright Epic Games, Inc. All Rights Reserved.

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Alias")
	bool bGlobalAlias;

	UPROPERTY()
	FString StateAliasName;

	// UObject
	virtual void Serialize(FArchive& Ar) override;

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void OnRenameNode(const FString& NewName) override;
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
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

	static FName GetAliasedStateNodesPropertyName() { return GET_MEMBER_NAME_CHECKED(URuleAliasNode, AliasedStateNodes); }

private:

	void RebuildAliasedStateNodeReferences();

	UPROPERTY()
	TSet<TWeakObjectPtr<URuleNodeBase>> AliasedStateNodes;
};
