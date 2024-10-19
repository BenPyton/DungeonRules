// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "RuleTransitionCondition.h"
#include "DungeonRulesLog.h"

bool URuleTransitionCondition::Check_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& PreviousRoom) const
{
	RulesLog_Error("Check is not implemented in %s.", *GetClass()->GetName());
	return false;
}

FText URuleTransitionCondition::GetDescription_Implementation() const
{
#if WITH_EDITOR
	return FText::Format(NSLOCTEXT("DungeonRules", "NoTransitionConditionDescription", "No condition description provided.\nOverride 'Get Description' in '{0}'."), GetClass()->GetDisplayNameText());
#else
	return FText();
#endif
}
