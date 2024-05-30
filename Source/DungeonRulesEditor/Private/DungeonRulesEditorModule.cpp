/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "DungeonRulesEditorModule.h"
#include "DungeonRulesEdLog.h"
#include "DungeonRulesEdTypes.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "EdGraphUtilities.h"
#include "Factories/DungeonRulesVisualFactories.h"

#define LOCTEXT_NAMESPACE "FDungeonRulesEditorModule"

void FDungeonRulesEditorModule::StartupModule()
{
	DungeonEd_LogInfo("DungeonRulesEditor module Startup");
	// Register visual factories for Dungeon Rules editor graph
	{
		DungeonRulesNodeFactory = MakeShareable(new FDungeonRulesNodeFactory());
		FEdGraphUtilities::RegisterVisualNodeFactory(DungeonRulesNodeFactory);

		DungeonRulesPinFactory = MakeShareable(new FDungeonRulesPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(DungeonRulesPinFactory);

		DungeonRulesPinConnectionFactory = MakeShareable(new FDungeonRulesPinConnectionFactory());
		FEdGraphUtilities::RegisterVisualPinConnectionFactory(DungeonRulesPinConnectionFactory);
	}
}

void FDungeonRulesEditorModule::ShutdownModule()
{
	// Unregister
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(DungeonRulesNodeFactory);
		FEdGraphUtilities::UnregisterVisualPinFactory(DungeonRulesPinFactory);
		FEdGraphUtilities::UnregisterVisualPinConnectionFactory(DungeonRulesPinConnectionFactory);
	}
	DungeonEd_LogInfo("DungeonRulesEditor module Shutdown");
}

/*void FDungeonRulesEditorModule::StartNodeHelper()
{
	if (!NodeHelper.IsValid())
	{
		NodeHelper = MakeShareable(new FGraphNodeClassHelper(UATNode::StaticClass()));
		FGraphNodeClassHelper::AddObservedBlueprintClasses(UATAction_BlueprintBase::StaticClass());
		FGraphNodeClassHelper::AddObservedBlueprintClasses(UATCondition_BlueprintBase::StaticClass());
		NodeHelper->UpdateAvailableBlueprintClasses();
	}
}*/

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonRulesEditorModule, DungeonRulesEditor)
