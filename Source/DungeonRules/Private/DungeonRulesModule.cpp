// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesModule.h"
#include "DungeonRulesLog.h"

#define LOCTEXT_NAMESPACE "FDungeonRulesModule"

void FDungeonRulesModule::StartupModule()
{
	RulesLog_Info("DungeonRules Module Startup!");
}

void FDungeonRulesModule::ShutdownModule()
{
	RulesLog_Info("DungeonRules Module Shutdown!");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonRulesModule, DungeonRules)
