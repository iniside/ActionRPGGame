// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameSystemEditor : ModuleRules
	{
		public GameSystemEditor(ReadOnlyTargetRules Target) : base(Target)
        {
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameSystem",
                    "GameSystem/Public",
                    "GameSystemEditor",
                    "GameSystemEditor/Public",
                    "GameWeapons",
                    "GameWeapons/Public",
                    "GameWeaponsEditor",
                    "GameWeaponsEditor/Public",
                    "AbilityFramework",
                    "AbilityFramework/Public",
                    "AbilityFrameworkEditor",
                    "AbilityFrameworkEditor/Public",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameInventorySystemEditor",
                    "GameInventorySystemEditor/Public",
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
                    "GameSystem/Private",
					"GameSystemEditor/Private",
                    "GameWeapons/Private",
                    "GameWeaponsEditor/Private",
                    "AbilityFramework/Private",
                    "AbilityFrameworkEditor/Private",
                    "GameInventorySystem/Private",
                    "GameInventorySystemEditor/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "Engine",
                    "CoreUObject",
                    "UnrealEd",
                    "BlueprintGraph",
                    "GraphEditor",
                    "PropertyEditor",
                    "SlateCore",
                    "Slate",
                    "EditorStyle",
                    "Kismet",
                    "KismetCompiler",
                    "AssetTools",
                    "MainFrame",
                    "InputCore",
                    "GameplayTasksEditor",
                    "GameSystem",
                    "AbilityFramework",
                    "GameWeapons",
                    "GameWeaponsEditor",
                    "GameInventorySystem",
                    "AbilityFrameworkEditor",
                    "GameInventorySystemEditor"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}