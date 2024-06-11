// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesFactory.h"
#include "DungeonRules.h"

UDungeonRulesFactory::UDungeonRulesFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDungeonRules::StaticClass();
}

UObject* UDungeonRulesFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UDungeonRules* Rules = NewObject<UDungeonRules>(InParent, InClass, InName, Flags);
	return Rules;
}
