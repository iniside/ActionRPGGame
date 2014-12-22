// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameAttributesEditor : ModuleRules
	{
		public GameAttributesEditor(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameAttributes",
                    "GameAttributes/Public",
                 //   "GameAttributesEditor",
                    "GameAttributesEditor/Public"
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GameAttributesEditor/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "Engine",
                    "CoreUObject",
                    "GameAttributes",
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
                    "GameplayTagsEditor"
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