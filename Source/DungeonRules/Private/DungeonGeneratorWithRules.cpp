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

#include "DungeonGeneratorWithRules.h"
#include "DungeonRules.h"
#include "DungeonRulesLog.h"

URoomData* ADungeonGeneratorWithRules::ChooseFirstRoomData_Implementation()
{
	if (!DungeonRules)
	{
		RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this));
		return nullptr;
	}

	if (!CurrentRule)
	{
		RulesLog_Error("Invalid Dungeon Rule in '%s'", *GetNameSafe(this));
		return nullptr;
	}

	URoomData* FirstRoom = DungeonRules->GetFirstRoomData(this, CurrentRule);
	CurrentRule = DungeonRules->GetNextRule(this, CurrentRule, FirstRoom);
	return FirstRoom;
}

URoomData* ADungeonGeneratorWithRules::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const FDoorDef& DoorData, int& DoorIndex)
{
	if (!DungeonRules)
	{
		RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this));
		return nullptr;
	}

	if (!CurrentRule)
	{
		RulesLog_Error("Invalid Dungeon Rule in '%s'", *GetNameSafe(this));
		return nullptr;
	}

	URoomData* NextRoom = DungeonRules->GetNextRoomData(this, CurrentRule, CurrentRoom, DoorData, DoorIndex);
	CurrentRule = DungeonRules->GetNextRule(this, CurrentRule, NextRoom);
	return NextRoom;
}

//TSubclassOf<ADoor> ADungeonGeneratorWithRules::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped)
//{
//}

//bool ADungeonGeneratorWithRules::IsValidDungeon_Implementation()
//{
//}

bool ADungeonGeneratorWithRules::ContinueToAddRoom_Implementation()
{
	if (!DungeonRules)
	{
		RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this));
		return false;
	}

	return CurrentRule != nullptr;
}

void ADungeonGeneratorWithRules::OnGenerationInit_Implementation()
{
	if (!DungeonRules)
	{
		RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this));
		return;
	}

	CurrentRule = DungeonRules->GetFirstRule();
}
