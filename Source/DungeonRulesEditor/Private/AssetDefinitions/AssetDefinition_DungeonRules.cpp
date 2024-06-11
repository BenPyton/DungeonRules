// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "AssetDefinition_DungeonRules.h"
#include "DungeonRules.h"
#include "Toolkits/DungeonRulesToolkit.h"
#include "Modules/ModuleManager.h"
//#include "ProceduralDungeonEditor.h"

#define LOCTEXT_NAMESPACE "ActionTreeEditor"

FText UAssetDefinition_DungeonRules::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_DungeonRules", "Dungeon Rules");
}

FLinearColor UAssetDefinition_DungeonRules::GetAssetColor() const
{
	return FColor(255, 50, 128);
}

TSoftClassPtr<UObject> UAssetDefinition_DungeonRules::GetAssetClass() const
{
	return UDungeonRules::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_DungeonRules::GetAssetCategories() const
{
	static const FAssetCategoryPath Categories[] = {FAssetCategoryPath(LOCTEXT("ProceduralDungeonCategory", "Procedural Dungeon"))};
	return Categories;
}

EAssetCommandResult UAssetDefinition_DungeonRules::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	//FProceduralDungeonEditorModule& DungeonEdModule = FModuleManager::GetModuleChecked<FProceduralDungeonEditorModule>("ProceduralDungeonEditorModule");
	//DungeonEdModule.StartNodeHelper();

	EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();
	for (UDungeonRules* DungeonRules : OpenArgs.LoadObjects<UDungeonRules>())
	{
		if (DungeonRules == nullptr)
			continue;

		TSharedRef<FDungeonRulesToolkit> NewEditor(new FDungeonRulesToolkit());
		NewEditor->InitGraphAssetEditor(Mode, OpenArgs.ToolkitHost, DungeonRules);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
