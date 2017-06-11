// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
    public ActionRPGGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.AddRange(
            new string[] {
                    "GameInterfaces",
                    "GameInterfaces/Public",
                    "GameSystem",
                    "GameSystem/Public",
                    "AbilityFramework",
                    "AbilityFramework/Public",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameWidgets",
                    "GameWidgets/Public",
                    "ActionRPGGame",
                    "ActionRPGGame/Public",
					// ... add public include paths required here ...
				}
            );

        PrivateIncludePaths.AddRange(
            new string[] {
					"GameSystem/Private",
                    "AbilityFramework/Private",
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
                "GameplayTasks",
                "GameplayTags",
                "GameInventorySystem", 
                "GameWidgets",
                "AbilityFramework",
                "GameWeapons",
                "GameSystem",
                "GameInterfaces",
                "ActorSequence",
                "RHI",
                "RenderCore",
                "ShaderCore"
                });
        PrivateDependencyModuleNames.AddRange(
        new string[]
				    {
					    // ... add private dependencies that you statically link with here ...
				    }
        );
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "PropertyEditor" });
        }
    }

}
