// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonInterfaces.generated.h"

class ADungeonGenerator;
class URoomData;
class UDungeonRule;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDungeonRuleProvider : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONRULES_API IDungeonRuleProvider
{
	GENERATED_BODY()
public:
	virtual const UDungeonRule* GetRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const = 0;
};

/////////////////////////////////////////

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDungeonConditionProvider : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONRULES_API IDungeonConditionProvider
{
	GENERATED_BODY()
public:
	virtual bool CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const = 0;
};
