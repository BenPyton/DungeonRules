// Copyright (c) 2025 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonDoorSelector.generated.h"

class URoom;
class UDoorType;
class ADoor;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class DUNGEONRULES_API UDungeonDoorSelector : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Dungeon Rules")
	TSubclassOf<ADoor> ChooseDoor(const URoom* RoomA, const URoom* RoomB, const UDoorType* DoorType, const UDoorType* OtherDoorType, bool& bFlipped) const;
};
