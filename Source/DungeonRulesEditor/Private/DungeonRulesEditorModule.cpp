// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesEditorModule.h"
#include "DungeonRulesEdLog.h"
#include "Nodes/RuleAliasNode.h"
#include "Factories/DungeonRulesVisualFactories.h"
#include "DetailCustomizations/DungeonRuleAliasNodeDetails.h"
#include "EdGraphUtilities.h"
#include "Styling/SlateStyleRegistry.h"
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
		FString ContentDir = IPluginManager::Get().FindPlugin("DungeonRules")->GetBaseDir();
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDungeonRulesEditorModule, DungeonRulesEditor)
