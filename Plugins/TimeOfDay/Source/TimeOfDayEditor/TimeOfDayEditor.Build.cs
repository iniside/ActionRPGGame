// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class TimeOfDayEditor : ModuleRules
	{
		public TimeOfDayEditor(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"TimeOfDayEditor/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "TimeOfDay"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
                   // "MainFrame",
			        "CoreUObject",
			        "Slate",
			        "SlateCore",
			        "Engine",
                    "InputCore",
			        "AssetTools",
			        "UnrealEd", // for FAssetEditorManager
			        "KismetWidgets",
			        "GraphEditor",
			        "Kismet",  // for FWorkflowCentricApplication
			        "PropertyEditor",
			        "RenderCore",
			        "LevelEditor", // for EdModes to get a toolkit host  //@TODO: PAPER: Should be a better way to do this (see the @todo in EdModeTileMap.cpp)
			        "Paper2D",
			        "ContentBrowser",
			        "WorkspaceMenuStructure",
			        "EditorStyle", 
                    "JsonUtilities",
                    "ClassViewer",
                    "AssetRegistry",
                    "ActorPickerMode",
                    "DetailCustomizations",
                    "AppFramework"
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