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

UCLASS(MinimalAPI, config=Editor)
class URuleTransitionNode : public URuleNodeBase
{
	GENERATED_BODY()

public:
	URuleTransitionNode();

#if false
	// The transition logic graph for this transition (returning a boolean)
	//UPROPERTY()
	TObjectPtr<class UEdGraph> BoundGraph;
#endif

	// The priority order of this transition. If multiple transitions out of a state go
	// true at the same time, the one with the smallest priority order will take precedent
	UPROPERTY(EditAnywhere, Category=Transition)
	int32 PriorityOrder;

#if false
	/** The rules for this transition may be shared with other transition nodes */
	//UPROPERTY()
	bool bSharedRules;

	/** What we call this transition if we are shared ('Transition X to Y' can't be used since its used in multiple places) */
	//UPROPERTY()
	FString	SharedRulesName;

	/** Shared rules guid useful when copying between different state machines */
	//UPROPERTY()
	FGuid SharedRulesGuid;

	/** Color we draw in the editor as if we are shared */
	//UPROPERTY()
	FLinearColor SharedColor;
#endif

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
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
	virtual void DestroyNode() override;
#if false
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual TArray<UEdGraph*> GetSubGraphs() const override;
#endif
	//~ End UEdGraphNode Interface

	//~ Begin URuleNodeBase Interface
#if false
	virtual UEdGraph* GetBoundGraph() const override { return BoundGraph; }
	virtual void ClearBoundGraph() override { BoundGraph = nullptr; }
#endif
	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	//~ End URuleNodeBase Interface

	// @return the name of this state
	DUNGEONRULESEDITOR_API FString GetStateName() const override;

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

#if false
	DUNGEONRULESEDITOR_API bool IsBoundGraphShared() const;

	DUNGEONRULESEDITOR_API void MakeRulesShareable(FString ShareName);
	DUNGEONRULESEDITOR_API void UnshareRules();

	DUNGEONRULESEDITOR_API void UseSharedRules(const URuleTransitionNode* Node);
#endif

	DUNGEONRULESEDITOR_API bool IsReverseTrans(const URuleNodeBase* Node);
protected:
#if false
	void CreateBoundGraph();
#endif

private:
	UPROPERTY(EditAnywhere, Instanced)
	TObjectPtr<class URuleTransitionCondition> Condition {nullptr};
};
