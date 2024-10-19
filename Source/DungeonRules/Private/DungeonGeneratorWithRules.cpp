// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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
	return FirstRoom;
}

URoomData* ADungeonGeneratorWithRules::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)
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

	DoorIndex = -1;
	URoomData* NextRoom = DungeonRules->GetNextRoomData(this, CurrentRule, CurrentRoomInstance, DoorData, DoorIndex);
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

void ADungeonGeneratorWithRules::OnRoomAdded_Implementation(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance)
{
	if (!DungeonRules)
	{
		RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this));
		return;
	}

	CurrentRule = DungeonRules->GetNextRule(this, CurrentRule, RoomInstance);
}
