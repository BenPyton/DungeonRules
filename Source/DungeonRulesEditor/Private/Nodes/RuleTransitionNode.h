// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "RuleNodeBase.h"
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
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PostPasteNode() override;
	//~ End UEdGraphNode Interface

	//~ Begin URuleNodeBase Interface
	DUNGEONRULESEDITOR_API virtual const UClass* GetInstanceClass() const override;
	DUNGEONRULESEDITOR_API virtual FString GetStateName() const override;
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	DUNGEONRULESEDITOR_API virtual TArray<FName> GetPropertyNamesToEdit() const override;
	//~ End URuleNodeBase Interface

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
};
