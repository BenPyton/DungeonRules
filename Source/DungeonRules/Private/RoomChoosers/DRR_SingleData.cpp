// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "RoomChoosers/DRR_SingleData.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "UDRR_SingleData"

URoomData* UDRR_SingleData::ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const
{
	return RoomData;
}

URoomData* UDRR_SingleData::ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const
{
	return RoomData;
}

FText UDRR_SingleData::GetDescription_Implementation() const
{
	return FText::Format(LOCTEXT("Description", "Return '{0}'"), FText::FromString(GetNameSafe(RoomData)));
}

#undef LOCTEXT_NAMESPACE
