// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonGeneratorWithRules.h"
#include "DungeonRules.h"
#include "DungeonRulesLog.h"

#define CHECK_RULES(RETURN_VALUE) \
if (!DungeonRules) \
{ \
	RulesLog_Error("Missing DungeonRules data in the Dungeon Generator '%s'", *GetNameSafe(this)); \
	return RETURN_VALUE; \
}

URoomData* ADungeonGeneratorWithRules::ChooseFirstRoomData_Implementation()
{
	CHECK_RULES(nullptr);

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
	CHECK_RULES(nullptr);

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

bool ADungeonGeneratorWithRules::IsValidDungeon_Implementation()
{
	CHECK_RULES(false);
	return DungeonRules->IsDungeonValid(this);
}

bool ADungeonGeneratorWithRules::ContinueToAddRoom_Implementation()
{
	CHECK_RULES(false);
	return CurrentRule != nullptr;
}

void ADungeonGeneratorWithRules::InitializeDungeon_Implementation(const UDungeonGraph* Rooms)
{
	CHECK_RULES();
	DungeonRules->InitializeDungeon(this, Rooms);
}

void ADungeonGeneratorWithRules::OnPreGeneration_Implementation()
{
	CHECK_RULES();
	DungeonRules->OnPreGeneration(this);
}

void ADungeonGeneratorWithRules::OnPostGeneration_Implementation()
{
	CHECK_RULES();
	DungeonRules->OnPostGeneration(this);
}

void ADungeonGeneratorWithRules::OnGenerationInit_Implementation()
{
	CHECK_RULES();
	DungeonRules->OnGenerationInit(this);
	CurrentRule = DungeonRules->GetFirstRule();
}

void ADungeonGeneratorWithRules::OnGenerationFailed_Implementation()
{
	CHECK_RULES();
	DungeonRules->OnGenerationFailed(this);
}

void ADungeonGeneratorWithRules::OnRoomAdded_Implementation(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance)
{
	CHECK_RULES();
	DungeonRules->OnRoomAdded(this, RoomInstance);
	CurrentRule = DungeonRules->GetNextRule(this, CurrentRule, RoomInstance);
}

void ADungeonGeneratorWithRules::OnFailedToAddRoom_Implementation(const URoomData* FromRoom, const FDoorDef& FromDoor)
{
	CHECK_RULES();
	DungeonRules->OnFailedToAddRoom(this, FromRoom, FromDoor);
}

#undef CHECK_RULES
