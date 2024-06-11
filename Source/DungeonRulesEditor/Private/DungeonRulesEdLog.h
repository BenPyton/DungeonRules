// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonRulesEditor, Verbose, All);

#define DungeonEd_LogInfo(Format, ...)\
	UE_LOG(LogDungeonRulesEditor, Log, TEXT(Format), ##__VA_ARGS__)

#define DungeonEd_LogWarning(Format, ...)\
	UE_LOG(LogDungeonRulesEditor, Warning, TEXT(Format), ##__VA_ARGS__)

#define DungeonEd_LogError(Format, ...)\
	UE_LOG(LogDungeonRulesEditor, Error, TEXT(Format), ##__VA_ARGS__)
