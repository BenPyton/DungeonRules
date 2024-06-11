// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonRules, Log, All);

#define RulesLog_Info(Format, ...) \
	UE_LOG(LogDungeonRules, Log, TEXT(Format), ##__VA_ARGS__)

#define RulesLog_Warning(Format, ...) \
	UE_LOG(LogDungeonRules, Warning, TEXT(Format), ##__VA_ARGS__)

#define RulesLog_Error(Format, ...) \
	UE_LOG(LogDungeonRules, Error, TEXT(Format), ##__VA_ARGS__)
