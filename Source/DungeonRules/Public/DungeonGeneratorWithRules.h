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
#include "DungeonGenerator.h"
#include "DungeonGeneratorWithRules.generated.h"

class UDungeonRule;
class UDungeonRules;

UCLASS(ClassGroup = "Procedural Dungeon")
class DUNGEONRULES_API ADungeonGeneratorWithRules : public ADungeonGenerator
{
	GENERATED_BODY()

public:
	//~ Begin ADungeonGenerator Interface
	virtual URoomData* ChooseFirstRoomData_Implementation() override;
	virtual URoomData* ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const FDoorDef& DoorData, int& DoorIndex) override;
	//virtual TSubclassOf<ADoor> ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped) override;
	//virtual bool IsValidDungeon_Implementation() override;
	virtual bool ContinueToAddRoom_Implementation() override;
	virtual void OnGenerationInit_Implementation() override;

	//~ End ADungeonGenerator Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Rules")
	TObjectPtr<UDungeonRules> DungeonRules {nullptr};

private:
	UPROPERTY(Transient)
	TObjectPtr<const UDungeonRule> CurrentRule {nullptr};
};
