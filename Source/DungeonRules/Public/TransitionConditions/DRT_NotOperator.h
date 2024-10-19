// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "RuleTransitionCondition.h"
#include "DRT_NotOperator.generated.h"

UCLASS(meta = (DisplayName = "Logical Operator (NOT)"))
class DUNGEONRULES_API UDRT_NotOperator : public URuleTransitionCondition
{
	GENERATED_BODY()

public:
	//~ Begin URuleTransitionCondition Interface
	virtual bool Check_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom) const override;
	virtual FText GetDescription_Implementation() const override;
	//~ End URuleTransitionCondition Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Transition Condition")
	TObjectPtr<URuleTransitionCondition> Condition;

#if WITH_DEV_AUTOMATION_TESTS
public:
	void SetCondition(URuleTransitionCondition* NewCondition) { Condition = NewCondition; }
#endif
};
