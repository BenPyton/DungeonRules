// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "TransitionConditions/DRT_LogicalOperator.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "DRT_LogicalOperator"

bool UDRT_LogicalOperator::Check_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	// If there is no condition, by default the transition pass.
	// So if there is no condition in an AND or an OR, the transition must pass too.
	if (Conditions.Num() <= 0)
		return true;

	const bool OperatorResult = static_cast<bool>(Operator);
	for (const URuleTransitionCondition* Condition : Conditions)
	{
		if (Condition->Check(Generator, PreviousRoom) != OperatorResult)
			return !OperatorResult;
	}
	return OperatorResult;
}

FText UDRT_LogicalOperator::GetDescription_Implementation() const
{
	switch (Operator)
	{
	case ELogicalOperator::OR:
		return LOCTEXT("Description", "True when at least one condition is met.");
	case ELogicalOperator::AND:
		return LOCTEXT("Description", "True when all conditions are met.");
	default:
		checkNoEntry();
	}
	return FText();
}

#undef LOCTEXT_NAMESPACE
