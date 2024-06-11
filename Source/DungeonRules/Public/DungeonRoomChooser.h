// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralDungeonTypes.h"
#include "DungeonRoomChooser.generated.h"

class URoomData;
class ADungeonGenerator;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DUNGEONRULES_API UDungeonRoomChooser : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeon Rules", meta = (DisplayName = "Choose First Room"))
	URoomData* ChooseFirstRoomData(ADungeonGenerator* Generator) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeon Rules", meta = (DisplayName = "Choose Next Room", ReturnDisplayName = "Room Data", AutoCreateRefTerm = "DoorIndex"))
	URoomData* ChooseNextRoomData(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeon Rules")
	FText GetDescription() const;
};
