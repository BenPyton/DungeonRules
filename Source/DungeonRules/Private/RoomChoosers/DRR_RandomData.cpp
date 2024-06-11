// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)


#include "RoomChoosers/DRR_RandomData.h"
#include "RoomData.h"
#include "DungeonGenerator.h"

#define LOCTEXT_NAMESPACE "UDRR_RandomData"

URoomData* UDRR_RandomData::ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const
{
	check(IsValid(Generator));
	return Generator->GetRandomRoomData(RoomList);
}

URoomData* UDRR_RandomData::ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const
{
	check(IsValid(Generator));
	return Generator->GetRandomRoomData(RoomList);
}

FText UDRR_RandomData::GetDescription_Implementation() const
{
	return LOCTEXT("Description", "Return a random (uniform) RoomData from a static array.");
}

#undef LOCTEXT_NAMESPACE
