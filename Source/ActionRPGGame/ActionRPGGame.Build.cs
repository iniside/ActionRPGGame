// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
	public ActionRPGGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.AddRange(
        new string[] {
                "AbilityFramework",
                "AbilityFramework/Abilities",
                "AbilityFramework/Attributes",
                "AbilityFramework/Effects",
                "AbilityFramework/Public"
            // ... add public include paths required here ...
            }
        );

            PrivateIncludePaths.AddRange(
            new string[] {
                    "AbilityFramework",
                    "AbilityFramework/Abilities",
                    "AbilityFramework/Attributes",
                    "AbilityFramework/Effects",
                    "AbilityFramework/Private",
                // ... add other private include paths required here ...
                }
            );
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "GameplayTags", "AbilityFramework" });
	}
}
