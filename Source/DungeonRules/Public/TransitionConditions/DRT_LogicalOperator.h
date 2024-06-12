// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "RuleTransitionCondition.h"
#include "DRT_LogicalOperator.generated.h"

UENUM()
enum class ELogicalOperator : uint8
{
	OR = 0,
	AND = 1,
};

UCLASS(meta = (DisplayName = "Logical Operator (AND/OR)"))
class DUNGEONRULES_API UDRT_LogicalOperator : public URuleTransitionCondition
{
	GENERATED_BODY()

public:
	//~ Begin URuleTransitionCondition Interface
	virtual bool Check_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const override;
	virtual FText GetDescription_Implementation() const override;
	//~ End URuleTransitionCondition Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Condition")
	ELogicalOperator Operator {ELogicalOperator::AND};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Transition Condition")
	TArray<TObjectPtr<URuleTransitionCondition>> Conditions;

#if WITH_DEV_AUTOMATION_TESTS
public:
	void SetConditions(const TArray<TObjectPtr<URuleTransitionCondition>>& NewConditions) { Conditions = NewConditions; }
	void SetOperator(ELogicalOperator NewOperator) { Operator = NewOperator; }
#endif
};
