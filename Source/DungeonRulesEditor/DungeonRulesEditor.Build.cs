// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

using UnrealBuildTool;

public class DungeonRulesEditor : ModuleRules
{
	public DungeonRulesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// Uncomment that to detect when there are missing includes in cpp files
		//bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Projects" });

		PrivateDependencyModuleNames.AddRange(new string[] {
				"ProceduralDungeon",
				"DungeonRules",
				"ApplicationCore",
				"CoreUObject",
				"UnrealEd",
				"SlateCore",
				"Slate",
				"Engine",
				"AssetDefinition",
				"GraphEditor",
				"PropertyEditor",
				//"KismetWidgets", // for SKismetLinearExpression
				"BlueprintGraph", // UEdGraphSchema_K2
				"ToolMenus",
			}
		);
	}
}
