// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGameEditor : ModuleRules
{
    public ActionRPGGameEditor(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicIncludePaths.AddRange(
			new string[] {
                "ActionRPGGame",
                "ActionRPGGame/Public"
				// ... add public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"ActionRPGGameEditor/Private",
					// ... add other private include paths required here ...
			}
		);
        PublicDependencyModuleNames.AddRange(new string[] 
		{ 
            "GameplayTags",
            "GameplayTagsEditor",
            "ActionRPGGame",
            "GameInventorySystem",
            "GameInventorySystemEditor",
            "AbilityFramework",
            "AbilityFrameworkEditor",
            "TimeOfDayEditor"
		});
        PrivateDependencyModuleNames.AddRange(new string[] 
		{
            //"MainFrame",
            "Core",
			"CoreUObject",
			"Slate",
			"SlateCore",
			"Engine",
            "InputCore",
			"UnrealEd", // for FAssetEditorManager
            "AssetTools",
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
            "ActorPickerMode"
		});

        PrivateIncludePathModuleNames.AddRange(
         new string[] {
			}
		);
    }
}
