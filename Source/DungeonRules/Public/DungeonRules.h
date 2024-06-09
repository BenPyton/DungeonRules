/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralDungeonTypes.h"
#include "Interfaces/NodeInterfaces.h"
#include "Interfaces/DungeonInterfaces.h"
#include "DungeonRules.generated.h"

class UDungeonRoomChooser;
class URuleTransitionCondition;
class ADungeonGenerator;
class URoomData;
class UDungeonRule;

UCLASS()
class DUNGEONRULES_API UDungeonRuleTransition : public UObject, public INodeTooltip
{
	GENERATED_BODY()

public:
	// The priority over other transitions.
	// The less this number is, the more priority the transition has.
	UPROPERTY(EditAnywhere, Category = "Transition")
	int32 PriorityOrder {0};

	UPROPERTY(EditAnywhere, Instanced, Category = "Transition")
	TObjectPtr<URuleTransitionCondition> Condition {nullptr};

	UPROPERTY()
	TScriptInterface<IDungeonRuleProvider> NextRule {nullptr};

public:
	bool CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const;

	//~ Begin INodeTooltip Interface
	virtual FText GetNodeTooltip() const override;
	//~ End INodeTooltip Interface

	static TOptional<const UDungeonRule*> GetNextRuleFromTransitionList(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const TArray<TWeakObjectPtr<const UDungeonRuleTransition>>& Transitions, const UObject* Context = nullptr);
};

/////////////////////////////////////////

UCLASS()
class DUNGEONRULES_API UDungeonRule : public UObject, public INodeName, public INodeTooltip, public IDungeonRuleProvider
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Dungeon Rule")
	TObjectPtr<UDungeonRoomChooser> RoomChooser {nullptr};

	UPROPERTY()
	TArray<TWeakObjectPtr<const UDungeonRuleTransition>> Transitions;

	// Displayed name of the rule
	UPROPERTY()
	FString RuleName;

	//~ Begin INodeName Interface
	virtual FString GetNodeName() const { return RuleName; }
	virtual void OnNodeRename(FString NewName) { RuleName = NewName; }
	//~ End INodeName Interface

	//~ Begin INodeTooltip Interface
	virtual FText GetNodeTooltip() const override;
	//~ End INodeTooltip Interface

	//~ Begin IDungeonRuleProvider Interface
	virtual const UDungeonRule* GetRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const override { return this; }
	//~ End IDungeonRuleProvider Interface

public:
	TOptional<const UDungeonRule*> GetNextRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const;

#if WITH_EDITOR
public:
	void Clear();
	void AddTransition(const UDungeonRuleTransition* Transition);
#endif
};

/////////////////////////////////////////

UCLASS()
class DUNGEONRULES_API URuleConduit : public UObject, public IDungeonRuleProvider, public IDungeonConditionProvider
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<TWeakObjectPtr<const UDungeonRuleTransition>> Transitions;

	//~ Begin IDungeonRuleProvider Interface
	virtual const UDungeonRule* GetRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const override;
	//~ End IDungeonRuleProvider Interface

	//~ Begin IDungeonConditionProvider Interface
	virtual bool CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const override;
	//~ End IDungeonConditionProvider Interface

#if WITH_EDITOR
public:
	void Clear();
	void AddTransition(const UDungeonRuleTransition* Transition);
#endif
};

/////////////////////////////////////////

UCLASS(BlueprintType)
class DUNGEONRULES_API UDungeonRules : public UDataAsset
{
	GENERATED_BODY()

public:
	UDungeonRules();

public:
	URoomData* GetFirstRoomData(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule) const;
	URoomData* GetNextRoomData(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const;
	const UDungeonRule* GetNextRule(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule, const URoomData* RoomData) const;
	FORCEINLINE const UDungeonRule* GetFirstRule() const { return FirstRule.Get(); }

#if WITH_EDITOR
public:
	// Clear FirstRule, Rules and all transitions
	void Clear();
	void AddRule(UDungeonRule* Rule);
	void AddConduit(URuleConduit* Conduit);
	void SetFirstRule(UDungeonRule* Rule);
	void AddTransition(UDungeonRuleTransition* Transition);
	void AddGlobalTransition(UDungeonRuleTransition* GlobalTransition);
#endif

#if WITH_EDITORONLY_DATA
public:
	UPROPERTY()
	class UEdGraph* EdGraph {nullptr};
#endif

private:
	UPROPERTY()
	TArray<TObjectPtr<UDungeonRule>> Rules;

	// TODO: maybe merge conduits with rules using a common interface? (something like a 'ITransitionContainer'?)
	UPROPERTY()
	TArray<TObjectPtr<URuleConduit>> Conduits;

	UPROPERTY()
	TArray<TObjectPtr<UDungeonRuleTransition>> Transitions;

	// The first rule to start the generation.
	// The rule must be registered in the Rules array too.
	UPROPERTY()
	TWeakObjectPtr<UDungeonRule> FirstRule;

	// Holds transitions from *any* state to another state.
	// The transition objects must be registered in the Transitions array too.
	UPROPERTY()
	TArray<TWeakObjectPtr<const UDungeonRuleTransition>> GlobalTransitions;
};
