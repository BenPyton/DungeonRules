// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ProceduralDungeonTypes.h"
#include "UObject/NameTypes.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_NEWER_THAN(5, 3, 0)
#define GC_PTR(VAR_TYPE) TObjectPtr<VAR_TYPE>
#else
#define GC_PTR(VAR_TYPE) VAR_TYPE*
#endif

#if UE_VERSION_OLDER_THAN(5, 1, 0)
using FStyleProvider = FEditorStyle;
#define STYLESET_NAME() FEditorStyle::GetStyleSetname()
#else
using FStyleProvider = FAppStyle;
#define STYLESET_NAME() FAppStyle::GetAppStyleSetName()
#endif

namespace DungeonRulesPinCategory
{
	const FName Transition(TEXT("Transition"));
	const FName Exec(TEXT("exec"));
}
