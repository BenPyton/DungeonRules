// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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
