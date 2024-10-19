// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "RuleTransitionCondition.h"
#include "DungeonRulesTypes.h"
#include "Templates/SubclassOf.h"
#include "DRT_RoomClassCount.generated.h"

UCLASS(meta = (DisplayName = "Room Count (Class)"))
class DUNGEONRULES_API UDRT_RoomClassCount : public URuleTransitionCondition
{
	GENERATED_BODY()

public:
	//~ Begin URuleTransitionCondition Interface
	virtual bool Check_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom) const override;
	virtual FText GetDescription_Implementation() const override;
	//~ End URuleTransitionCondition Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Condition")
	EComparisonOp Comparison {EComparisonOp::Equal};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Condition")
	int Count {1};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Condition")
	TArray<TSubclassOf<URoomData>> RoomClassToCount {};
};
