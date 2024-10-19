// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "TransitionConditions/DRT_RoomDataCount.h"
#include "DungeonGenerator.h"
#include "DungeonGraph.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "DRT_RoomDataCount"

bool UDRT_RoomDataCount::Check_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom) const
{
	check(IsValid(Generator));
	const int Result = (RoomDataToCount.Num() > 0)
		? Generator->GetRooms()->CountTotalRoomData(RoomDataToCount)
		: Generator->GetRooms()->Count();

	return FComparisonHelper::Check(Result, Count, Comparison);
}

FText UDRT_RoomDataCount::GetDescription_Implementation() const
{
	FText CompareText = FComparisonHelper::GetComparisonText(Comparison, Count);
	FText DataText;
	if (RoomDataToCount.Num() == 1)
		DataText = FText::Format(LOCTEXT("DescriptionSingleData", " with data '{0}'"), FText::FromString(GetNameSafe(RoomDataToCount[0])));
	else if (RoomDataToCount.Num() > 1)
		DataText = LOCTEXT("DescriptionMultipleData", " from a collection of data");

	return FText::Format(LOCTEXT("Description", "True when the dungeon has {0} room(s){1}."), CompareText, DataText);
}

#undef LOCTEXT_NAMESPACE
