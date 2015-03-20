// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
	public ActionRPGGame(TargetInfo Target)
	{
        PublicIncludePaths.AddRange(
            new string[] {
                    "GameInterfaces",
                    "GameInterfaces/Public",
                    "GameSystem",
                    "GameSystem/Public",
                    "GameAbilities",
                    "GameAbilities/Public",
                    "GameAttributes",
					"GameAttributes/Effects",
                    "GameAttributes/Public",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameFloatingCombatText",
                    "GameFloatingCombatText/Public",
                    "ActionRPGGame",
                    "ActionRPGGame/Public",
					// ... add public include paths required here ...
				}
            );

        PrivateIncludePaths.AddRange(
            new string[] {
					"GameSystem/Private",
                    "GameAbilities/Private",
                    "GameAttributes/Private",
                    "GameInventorySystem/Private",
                    "ActionRPGGame/Private",
					// ... add other private include paths required here ...
				}
            );
		PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
                "UMG",
                "Slate",
                "SlateCore",
                "GameInventorySystem", 
				"GameAttributes",
				"GameplayTags",
                "GameFloatingCombatText",
                "GameAbilities",
                "GameSystem",
                "GameInterfaces"
				});
		if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "PropertyEditor", "GameAttributesEditor"});
        }
	}

}
