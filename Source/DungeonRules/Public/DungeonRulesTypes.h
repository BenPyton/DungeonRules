// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "DungeonRulesTypes.generated.h"

UENUM()
enum class EComparisonOp : uint8
{
	Equal			= 0b001		UMETA(DisplayName = "Equal"),
	NotEqual		= 0b110		UMETA(DisplayName = "Not Equal"),
	Less			= 0b010		UMETA(DisplayName = "Less"),
	LessEqual		= 0b011		UMETA(DisplayName = "Less or Equal"),
	Greater			= 0b100		UMETA(DisplayName = "Greater"),
	GreaterEqual	= 0b101		UMETA(DisplayName = "Greater or Equal"),
};

struct FComparisonHelper
{
	DUNGEONRULES_API static bool Check(const int A, const int B, const EComparisonOp Operator);
	DUNGEONRULES_API static FText GetComparisonText(const EComparisonOp Operator, const int Value);
};
