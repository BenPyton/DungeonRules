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
