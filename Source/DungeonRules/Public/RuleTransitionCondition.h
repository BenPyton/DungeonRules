// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RuleTransitionCondition.generated.h"

class URoomData;
class ADungeonGenerator;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DUNGEONRULES_API URuleTransitionCondition : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeon Rules")
	bool Check(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeon Rules")
	FText GetDescription() const;
};
