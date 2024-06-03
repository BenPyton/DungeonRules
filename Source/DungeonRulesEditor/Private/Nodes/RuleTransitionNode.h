// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Guid.h"
//#include "Animation/AnimTypes.h"
//#include "Animation/RuleMachineTypes.h"
//#include "Animation/BlendProfile.h"
#include "RuleNodeBase.h"
//#include "RuleTransitionCondition.h"
#include "RuleTransitionNode.generated.h"

class UCurveFloat;
class UEdGraph;
class UEdGraphPin;
class UDungeonRuleTransition;

UCLASS(MinimalAPI, config=Editor)
class URuleTransitionNode : public URuleNodeBase
{
	GENERATED_BODY()

public:
	URuleTransitionNode();

	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	//~ End UEdGraphNode Interface

	//~ Begin URuleNodeBase Interface
	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	//~ End URuleNodeBase Interface

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

	virtual void PostCopyNode() override;
	void ResetInstanceOwner();

	// @return the name of this state
	DUNGEONRULESEDITOR_API FString GetStateName() const override;

	// @return The description of the condition
	FText GetConditionDescription() const;

	DUNGEONRULESEDITOR_API URuleNodeBase* GetPreviousState() const;
	DUNGEONRULESEDITOR_API URuleNodeBase* GetNextState() const;
	DUNGEONRULESEDITOR_API void CreateConnections(URuleNodeBase* PreviousState, URuleNodeBase* NextState);

	/**
	 * Relink transition head (where the arrow is of a state transition) to a new state.
	 * @param[in] NewTargetState The new transition target.
	 */
	DUNGEONRULESEDITOR_API void RelinkHead(URuleNodeBase* NewTargetState);

	/**
	 * Helper function to gather the transition nodes to be relinked by taking the graph selection into account as well.
	 * For example when relinking a transition holding several transition nodes but only a few are selected to be relinked.
	 */
	DUNGEONRULESEDITOR_API static TArray<URuleTransitionNode*> GetListTransitionNodesToRelink(UEdGraphPin* SourcePin, UEdGraphPin* OldTargetPin, const TArray<UEdGraphNode*>& InSelectedGraphNodes);

	UDungeonRuleTransition* GetNodeInstance() const { return NodeInstance; }

private:
	void CreateInstance(const UDungeonRuleTransition* Template = nullptr);

private:
	UPROPERTY()
	TObjectPtr<UDungeonRuleTransition> NodeInstance {nullptr};
};
