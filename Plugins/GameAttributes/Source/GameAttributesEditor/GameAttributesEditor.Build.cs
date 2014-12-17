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
                    "GameAttributes/Public"
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
                    "GameAttributes"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				    "Core",
				    "CoreUObject",
				    "InputCore",
				    "Slate",
				    "Engine",
				    "AssetTools",
				    "UnrealEd", // for FAssetEditorManager
				    "KismetWidgets",
				    "KismetCompiler",
				    "BlueprintGraph",
				    "GraphEditor",
				    "Kismet",  // for FWorkflowCentricApplication
				    "PropertyEditor",
				    "EditorStyle",
				    "Slate",
				    "SlateCore",
				    "MovieSceneCore",
				    "Sequencer",
				    "DetailCustomizations",
                    "Settings",
				    "RenderCore",
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