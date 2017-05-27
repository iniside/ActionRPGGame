// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameInventorySystemEditor : ModuleRules
	{
		public GameInventorySystemEditor(ReadOnlyTargetRules Target) : base(Target)
        {
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameInventorySystem",
                    "GameInventorySystem/Public"
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GameInventorySystemEditor/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "GameInventorySystem",
                    "GraphEditor",
                    "MainFrame",
                    "EditorStyle",
                    "CoreUObject",
				    "InputCore",
				    "Slate",
				    "Engine",
				    "AssetTools",
				    "UnrealEd", // for FAssetEditorManager
				    "KismetWidgets",
				    "KismetCompiler",
				    "BlueprintGraph",
				    "Kismet",  // for FWorkflowCentricApplication
				    "PropertyEditor",
				    
				    "Slate",
				    "SlateCore",
				    "DetailCustomizations",
                    "Settings",
				    "RenderCore",
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				    "Core",

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