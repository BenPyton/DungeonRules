// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRoomChooser.h"
#include "DRR_SingleData.generated.h"

UCLASS(meta = (DisplayName = "Single Data"))
class DUNGEONRULES_API UDRR_SingleData : public UDungeonRoomChooser
{
	GENERATED_BODY()

public:
	//~ Begin UDungeonRoomChooser Interface
	virtual URoomData* ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const override;
	virtual URoomData* ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const override;
	virtual FText GetDescription_Implementation() const override;
	//~ End UDungeonRoomChooser Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Chooser")
	TObjectPtr<URoomData> RoomData {nullptr};
};
