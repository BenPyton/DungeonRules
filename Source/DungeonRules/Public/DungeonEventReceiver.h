// Copyright (c) 2024 Chat Cripant

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonEventReceiver.generated.h"

class ADungeonGenerator;
class URoom;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class DUNGEONRULES_API UDungeonEventReceiver : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnPreGeneration(ADungeonGenerator* Generator);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnPostGeneration(ADungeonGenerator* Generator);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnGenerationInit(ADungeonGenerator* Generator);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnGenerationFailed(ADungeonGenerator* Generator);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnRoomAdded(ADungeonGenerator* Generator, const URoomData* NewRoom);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnFailedToAddRoom(ADungeonGenerator* Generator, const URoomData* FromRoom, const FDoorDef& FromDoor);
};
