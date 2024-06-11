// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DungeonRulesFactory.generated.h"

UCLASS()
class UDungeonRulesFactory : public UFactory
{
	GENERATED_BODY()

public:
	UDungeonRulesFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
	//~ End UFactory Interface
};
