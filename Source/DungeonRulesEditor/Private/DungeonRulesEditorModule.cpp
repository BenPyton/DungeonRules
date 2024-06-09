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
#include "DetailCustomizations/DungeonRuleAliasNodeDetails.h"
#include "Nodes/RuleAliasNode.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FDungeonRulesEditorModule"

#define REGISTER_CUSTOM_CLASS_DETAILS(TYPE, DETAILS)\
	PropertyModule.RegisterCustomClassLayout(TYPE::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&DETAILS::MakeInstance));

#define UNREGISTER_CUSTOM_CLASS_DETAILS(TYPE)\
	PropertyModule.UnregisterCustomClassLayout(TYPE::StaticClass()->GetFName());

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( StyleSet->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

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

	// Register detail customizations
	{
		auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		REGISTER_CUSTOM_CLASS_DETAILS(URuleAliasNode, FDungeonRuleAliasNodeDetails);
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	// Register slate style set
	{
		StyleSet = MakeShareable(new FSlateStyleSet("DungeonRulesStyle"));
		FString ContentDir = IPluginManager::Get().FindPlugin("ProceduralDungeon")->GetBaseDir();
		StyleSet->SetContentRoot(ContentDir);

		const FVector2D Icon20x20(20.0f, 20.0f);
		const FVector2D Icon40x40(40.0f, 40.0f);

		StyleSet->Set("DungeonRules.ConduitNode.Body", new IMAGE_BRUSH(TEXT("Resources/ConduitNode_body"), Icon40x40));
		StyleSet->Set("DungeonRules.ConduitNode.Shadow", new IMAGE_BRUSH(TEXT("Resources/ConduitNode_shadow"), Icon40x40));
		StyleSet->Set("DungeonRules.ConduitNode.ShadowSelected", new IMAGE_BRUSH(TEXT("Resources/ConduitNode_shadow_selected"), Icon40x40));
		StyleSet->Set("DungeonRules.ConduitNode.Pin.Hovered", new IMAGE_BRUSH(TEXT("Resources/ConduitNode_pin_hovercue"), Icon40x40));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FDungeonRulesEditorModule::ShutdownModule()
{
	// Unregister visual factories
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(DungeonRulesNodeFactory);
		FEdGraphUtilities::UnregisterVisualPinFactory(DungeonRulesPinFactory);
		FEdGraphUtilities::UnregisterVisualPinConnectionFactory(DungeonRulesPinConnectionFactory);
	}

	// Unregister detail customizations
	{
		auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		UNREGISTER_CUSTOM_CLASS_DETAILS(URuleAliasNode);
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	// Unregister slate style set
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
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
