// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "RuleTransitionCondition.h"
#include "TransitionConditionTestClasses.generated.h"

#if !WITH_DEV_AUTOMATION_TESTS
static_assert("Do not include this file outside of unit tests!");
#endif

// Utility to create room data
#define CREATE_CONDITION_INSTANCE(TYPE, NAME) \
	TStrongObjectPtr<TYPE> NAME(NewObject<TYPE>(GetTransientPackage(), #NAME));

// Transition condition that always return true.
UCLASS()
class UDRT_True : public URuleTransitionCondition
{
	GENERATED_BODY()

public:
	virtual bool Check_Implementation(ADungeonGenerator*, const TScriptInterface<IReadOnlyRoom>&) const override { return true; }
};

// Transition condition that always return false.
UCLASS()
class UDRT_False : public URuleTransitionCondition
{
	GENERATED_BODY()

public:
	virtual bool Check_Implementation(ADungeonGenerator*, const TScriptInterface<IReadOnlyRoom>&) const override { return false; }
};
