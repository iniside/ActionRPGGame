// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGameEditor : ModuleRules
{
	public ActionRPGGameEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
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
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore",
            "UMG",
            "GameplayTags",
            "AbilityFramework",
            "UnrealEd", "SourceControl", "Matinee", "PropertyEditor", "ShaderCore", "AbilityFrameworkEditor"
        });
           
        PrivateDependencyModuleNames.AddRange(new string[] { "AbilityFrameworkEditor" });
        
    }
}
