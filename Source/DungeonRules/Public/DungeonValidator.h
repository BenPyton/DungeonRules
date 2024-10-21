// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

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
