// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "RuleTransitionCondition.h"
#include "DungeonRulesTypes.h"
#include "DRT_RoomDataCount.generated.h"

UCLASS(meta = (DisplayName = "Room Count (Data)"))
class DUNGEONRULES_API UDRT_RoomDataCount : public URuleTransitionCondition
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
	TArray<TObjectPtr<URoomData>> RoomDataToCount {};
};
