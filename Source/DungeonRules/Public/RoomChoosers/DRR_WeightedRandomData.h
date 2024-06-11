// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRoomChooser.h"
#include "DRR_WeightedRandomData.generated.h"

USTRUCT(BlueprintType)
struct FRoomWeightPair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Chooser")
	TObjectPtr<URoomData> RoomData {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Chooser")
	int Weight {1};
};

UCLASS(meta = (DisplayName = "Random Data (Weighted)"))
class DUNGEONRULES_API UDRR_WeightedRandomData : public UDungeonRoomChooser
{
	GENERATED_BODY()

public:
	//~ Begin UDungeonRoomChooser Interface
	virtual URoomData* ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const override;
	virtual URoomData* ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const override;
	virtual FText GetDescription_Implementation() const override;
	//~ End UDungeonRoomChooser Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Chooser")
	TArray<FRoomWeightPair> WeightedRoomList {};
};
