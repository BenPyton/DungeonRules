// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimTypes.h"
#include "RuleNodeBase.h"
#include "RuleNode.generated.h"

class UEdGraph;
class UEdGraphPin;
class UDungeonRule;

UCLASS(MinimalAPI)
class URuleNode : public URuleNodeBase
{
	GENERATED_BODY()
public:
	URuleNode();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual void OnRenameNode(const FString& NewName) override;
	//~ End UEdGraphNode Interface
	
	//~ Begin URuleNodeBase Interface
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	//~ End URuleNodeBase Interface

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

public:
	FORCEINLINE UDungeonRule* GetRuleInstance() const { return RuleInstance; }

	virtual void PostCopyNode() override;
	void ResetInstanceOwner();

private:
	void CreateInstance(const UDungeonRule* Template = nullptr);

private:
	UPROPERTY()
	TObjectPtr<UDungeonRule> RuleInstance {nullptr};
};
