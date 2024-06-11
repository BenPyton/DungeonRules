// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "RuleNodeBase.generated.h"

class INameValidatorInterface;
class UAnimBlueprint;
class UEdGraph;
class UEdGraphPin;
class UEdGraphSchema;

UCLASS(MinimalAPI, abstract)
class URuleNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	URuleNodeBase();

	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;

	//~ Begin UEdGraphNode Interface
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual FText GetTooltipText() const override;
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FString GetDocumentationLink() const override;
	//~ End UEdGraphNode Interface

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

	DUNGEONRULESEDITOR_API virtual const UClass* GetInstanceClass() const { return nullptr; }

	// @return the input pin for this state
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetInputPin() const { return nullptr; }

	// @return the output pin for this state
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetOutputPin() const { return nullptr; }

	// @return the name of this state
	DUNGEONRULESEDITOR_API virtual FString GetStateName() const { return TEXT("BaseState"); }

	// Populates the OutTransitions array with a list of transition nodes connected to this state
	DUNGEONRULESEDITOR_API virtual void GetTransitionList(TArray<class URuleTransitionNode*>& OutTransitions, bool bWantSortedList = false) const;

	DUNGEONRULESEDITOR_API virtual void PostCopyNode();

	// @return All names of the instance's properties to show in the detail panel
	DUNGEONRULESEDITOR_API virtual TArray<FName> GetPropertyNamesToEdit() const { return {}; }

	FORCEINLINE UObject* GetNodeInstance() const { return NodeInstance; }

	template<class T>
	T* GetNodeInstance() const
	{
		return Cast<T>(GetNodeInstance());
	}

protected:
	DUNGEONRULESEDITOR_API virtual FString GetDesiredNewNodeName() const { return TEXT("Base"); }
	DUNGEONRULESEDITOR_API virtual void InitializeInstance() const {}

private:
	void CreateInstance(bool bDuplicateInstance = false);
	void ResetInstanceOwner();

protected:
	UPROPERTY()
	TObjectPtr<UObject> NodeInstance {nullptr};
};
