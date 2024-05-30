// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimTypes.h"
#include "RuleNodeBase.h"
#include "RuleNode.generated.h"

class UEdGraph;
class UEdGraphPin;

UCLASS(MinimalAPI)
class URuleNode : public URuleNodeBase
{
	GENERATED_BODY()
public:
	URuleNode();

#if false
	// The animation graph for this state
	//UPROPERTY()
	TObjectPtr<class UEdGraph> BoundGraph;
#endif

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void DestroyNode() override;
#if false
	virtual TArray<UEdGraph*> GetSubGraphs() const override { return TArray<UEdGraph*>( { BoundGraph } ); }
#endif
	virtual void OnRenameNode(const FString& NewName) override;
	//~ End UEdGraphNode Interface
	
	//~ Begin URuleNodeBase Interface
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
	virtual FString GetStateName() const override;
	//~ End URuleNodeBase Interface

#if false
public:
	virtual UEdGraph* GetBoundGraph() const override { return BoundGraph; }
	virtual void ClearBoundGraph() override { BoundGraph = nullptr; }
#endif

private:
	UPROPERTY()
	FString NodeName;

	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<class UDungeonRoomChooser> RoomChooser {nullptr};
};
