// Copyright (c) 2024 Chat Cripant

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DungeonValidator.generated.h"

class ADungeonGenerator;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class DUNGEONRULES_API UDungeonValidator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Dungeon Rules")
	bool IsDungeonValid(const ADungeonGenerator* Generator) const;
};
