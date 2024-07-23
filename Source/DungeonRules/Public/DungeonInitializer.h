// Copyright (c) 2024 Chat Cripant

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonInitializer.generated.h"

class ADungeonGenerator;
class UDungeonGraph;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class DUNGEONRULES_API UDungeonInitializer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeon Rules")
	void InitializeDungeon(const ADungeonGenerator* Generator, const UDungeonGraph* Rooms) const;
};
