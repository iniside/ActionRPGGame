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
                "OrionAnimation",
                "OrionAnimation/Private",
                "OrionAnimation/Public",
                "AbilityFramework/Abilities",
                "AbilityFramework/Attributes",
                "AbilityFramework/Effects",
                "AbilityFramework/Public",
                "ActionRPGGame/Abilities",
                "ActionRPGGame/AI",
                "ActionRPGGame/Attributes",
                "ActionRPGGame/UI",
                "ActionRPGGame/UI/Abilities"
            // ... add public include paths required here ...
            }
        );

            PrivateIncludePaths.AddRange(
            new string[] {
                    "AbilityFramework",
                    "OrionAnimation",
                    "OrionAnimation/Private",
                "OrionAnimation/Public",
                    "AbilityFramework/Abilities",
                    "AbilityFramework/Attributes",
                    "AbilityFramework/Effects",
                    "AbilityFramework/Private",
                    "ActionRPGGame/Abilities",
                    "ActionRPGGame/AI",
                    "ActionRPGGame/Attributes",
                    "ActionRPGGame/UI",
                    "ActionRPGGame/UI/Abilities"
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
            "AssetRegistry",
            "OrionAnimation",
            //"Sockets",
            //"OnlineSubsystemUtils",
            "ActorSequence",
            "AbilityManager",
            "DraggableWindow"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Noesis", "NoesisRuntime" });
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "SourceControl", "Matinee", "PropertyEditor", "ShaderCore", "AbilityFrameworkEditor" });
            PrivateDependencyModuleNames.AddRange(new string[] { "AbilityFrameworkEditor" });
        }

        if (Target.Type == TargetRules.TargetType.Server)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "GameLiftServerSDK" });
        }
    }
}
