// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonGenerator.h"
#include "DungeonGeneratorWithRules.generated.h"

class UDungeonRule;
class UDungeonRules;

UCLASS(ClassGroup = "Procedural Dungeon", meta = (KismetHideOverrides = "ChooseFirstRoomData,ChooseNextRoomData,ContinueToAddRoom"))
class DUNGEONRULES_API ADungeonGeneratorWithRules : public ADungeonGenerator
{
	GENERATED_BODY()

public:
	//~ Begin ADungeonGenerator Interface
	virtual URoomData* ChooseFirstRoomData_Implementation() override;
	virtual URoomData* ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex) override;
	//virtual TSubclassOf<ADoor> ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped) override;
	virtual bool IsValidDungeon_Implementation() override;
	virtual bool ContinueToAddRoom_Implementation() override;
	virtual void InitializeDungeon_Implementation(const UDungeonGraph* Rooms) override;
	virtual void OnPreGeneration_Implementation() override;
	virtual void OnPostGeneration_Implementation() override;
	virtual void OnGenerationInit_Implementation() override;
	virtual void OnGenerationFailed_Implementation() override;
	virtual void OnRoomAdded_Implementation(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance) override;
	virtual void OnFailedToAddRoom_Implementation(const URoomData* FromRoom, const FDoorDef& FromDoor) override;
	//~ End ADungeonGenerator Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon Rules")
	TObjectPtr<UDungeonRules> DungeonRules {nullptr};

private:
	UPROPERTY(Transient)
	TObjectPtr<const UDungeonRule> CurrentRule {nullptr};
};
