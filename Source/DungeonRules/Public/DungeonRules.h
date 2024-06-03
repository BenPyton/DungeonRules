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
#include "DungeonRules.generated.h"

class UDungeonRoomChooser;
class URuleTransitionCondition;
class ADungeonGenerator;
class URoomData;

USTRUCT()
struct DUNGEONRULES_API FDungeonRuleTransition
{
	GENERATED_BODY()

public:
	// The priority over other transitions.
	// The less this number is, the more priority the transition has.
	UPROPERTY(EditAnywhere, Category = "Transition")
	int32 Priority {0};

	UPROPERTY(EditAnywhere, Instanced, Category = "Transition")
	URuleTransitionCondition* Condition {nullptr};

	UPROPERTY()
	TWeakObjectPtr<class UDungeonRule> NextRule {nullptr};

public:
	bool CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const;
};

UCLASS()
class DUNGEONRULES_API UDungeonRule : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Dungeon Rule")
	TObjectPtr<UDungeonRoomChooser> RoomChooser {nullptr};

	UPROPERTY()
	TArray<FDungeonRuleTransition> Transitions;

	// Displayed name of the rule
	UPROPERTY()
	FString RuleName;

public:
	const UDungeonRule* GetNextRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const;
};

/**
 * 
 */
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
	FORCEINLINE const UDungeonRule* GetFirstRule() const { return FirstRule; }

#if WITH_EDITOR
public:
	// Clear FirstRule, Rules and all transitions
	void Clear();
	void AddRule(UDungeonRule* Rule);
	void SetFirstRule(UDungeonRule* Rule);
#endif

#if WITH_EDITORONLY_DATA
public:
	UPROPERTY()
	class UEdGraph* EdGraph {nullptr};
#endif

private:
	UPROPERTY()
	TArray<TObjectPtr<UDungeonRule>> Rules;

	UPROPERTY()
	TObjectPtr<UDungeonRule> FirstRule;
};
