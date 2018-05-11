// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
    public ActionRPGGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");
        //PublicDefinitions.Add("_HAS_EXCEPTIONS 0");
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
            "Json",
            "JsonUtilities",
            "UMG",
            "GameplayTags",
            "AbilityFramework",
            "AssetRegistry",
            "OrionAnimation",
            //"Sockets",
            //"OnlineSubsystemUtils",
            "ActorSequence",
            "AbilityManager",
            "DraggableWindow",
            "JsonUObject",
            "InventoryFramework",
            "InventoryFrameworkUI",
            "GameSparks",
            "OnlineSubsystem",
            "OnlineSubsystemGameSparks"
        });

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "SourceControl", "Matinee", "PropertyEditor", "ShaderCore", "AbilityFrameworkEditor" });
            PrivateDependencyModuleNames.AddRange(new string[] { "AbilityFrameworkEditor" });
        }
        if (Target.Type == TargetRules.TargetType.Server)
        {
            if (Target.Platform == UnrealTargetPlatform.Linux)
            {
                PublicDependencyModuleNames.AddRange(new string[] { "GameLiftServerSDK" });
            }
        }
    }
}
