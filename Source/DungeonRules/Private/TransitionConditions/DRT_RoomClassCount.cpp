// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "TransitionConditions/DRT_RoomClassCount.h"
#include "DungeonGenerator.h"
#include "DungeonGraph.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "DRT_RoomClassCount"

bool UDRT_RoomClassCount::Check_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	check(IsValid(Generator));
	const int Result = (RoomClassToCount.Num() > 0)
		? Generator->GetRooms()->CountTotalRoomType(RoomClassToCount)
		: Generator->GetRooms()->Count();

	return FComparisonHelper::Check(Result, Count, Comparison);
}

FText UDRT_RoomClassCount::GetDescription_Implementation() const
{
	FText CompareText = FComparisonHelper::GetComparisonText(Comparison, Count);
	FText ClassText;
	if (RoomClassToCount.Num() == 1)
		ClassText = FText::Format(LOCTEXT("DescriptionSingleClass", " with class '{0}'"), FText::FromString(GetNameSafe(RoomClassToCount[0])));
	else if (RoomClassToCount.Num() > 1)
		ClassText = LOCTEXT("DescriptionMultipleClass", " from a collection of class");

	return FText::Format(LOCTEXT("Description", "True when the dungeon has {0} room(s){1}."), CompareText, ClassText);
}

#undef LOCTEXT_NAMESPACE
