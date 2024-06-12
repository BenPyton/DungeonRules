// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "TransitionConditions/DRT_NotOperator.h"
#include "RoomData.h"

#define LOCTEXT_NAMESPACE "DRT_NotOperator"

bool UDRT_NotOperator::Check_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	return Condition && !Condition->Check(Generator, PreviousRoom);
}

FText UDRT_NotOperator::GetDescription_Implementation() const
{
	if (!Condition)
		return LOCTEXT("Description", "Always false.");

	FText ChildConditionDescription = Condition->GetDescription();
	return FText::Format(LOCTEXT("Description", "True when this condition is false:\n- {0}"), ChildConditionDescription);
}

#undef LOCTEXT_NAMESPACE
