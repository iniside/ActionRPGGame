// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
	public ActionRPGGame(TargetInfo Target)
	{
        PublicIncludePaths.AddRange(
         new string[] {
				// ... add public include paths required here ...
			}
        );

        PrivateIncludePaths.AddRange(
            new string[] {
					// ... add other private include paths required here ...
			}
        );
		PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"GameInventorySystem", 
				"GameAttributes",
                "GameEffectSystem",
				"GameplayTags",
                "GameplayAbilities",
                "UMG",
                "Slate",
                "SlateCore",
				});
		if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "PropertyEditor" });
        }
	}

}
