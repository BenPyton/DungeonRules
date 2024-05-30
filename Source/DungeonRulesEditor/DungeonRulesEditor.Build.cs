// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

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
				"KismetWidgets", // for SKismetLinearExpression
				"BlueprintGraph", // UEdGraphSchema_K2
				"ToolMenus",
			}
		);
	}
}
