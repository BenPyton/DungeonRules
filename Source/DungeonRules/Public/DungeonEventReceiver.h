// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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
	void OnRoomAdded(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& NewRoom);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeon Rules")
	void OnFailedToAddRoom(ADungeonGenerator* Generator, const URoomData* FromRoom, const FDoorDef& FromDoor);
};
