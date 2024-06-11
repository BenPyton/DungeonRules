// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)


#include "RoomChoosers/DRR_WeightedRandomData.h"
#include "RoomData.h"
#include "DungeonGenerator.h"

#define LOCTEXT_NAMESPACE "UDRR_WeightedRandomData"

URoomData* UDRR_WeightedRandomData::ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const
{
	check(IsValid(Generator));

	TMap<URoomData*, int> WeightedMap;
	for (const auto& Pair : WeightedRoomList)
	{
		WeightedMap.Add(Pair.RoomData, Pair.Weight);
	}

	return Generator->GetRandomRoomDataWeighted(WeightedMap);
}

URoomData* UDRR_WeightedRandomData::ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const
{
	check(IsValid(Generator));

	TMap<URoomData*, int> WeightedMap;
	for (const auto& Pair : WeightedRoomList)
	{
		WeightedMap.Add(Pair.RoomData, Pair.Weight);
	}

	return Generator->GetRandomRoomDataWeighted(WeightedMap);
}

FText UDRR_WeightedRandomData::GetDescription_Implementation() const
{
	return LOCTEXT("Description", "Return a random (weighted) RoomData from a static array.");
}

#undef LOCTEXT_NAMESPACE

