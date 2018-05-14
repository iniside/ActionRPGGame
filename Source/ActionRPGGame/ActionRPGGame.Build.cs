// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
    public ActionRPGGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");
        
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
                PublicDefinitions.Add("LINUX_GAMELIFT=1");
                PublicDefinitions.Add("GAMESPARKS_SERVER=1");
                PublicDependencyModuleNames.AddRange(new string[] { "GameLiftServerSDK" });
            }
            else
            {
                PublicDefinitions.Add("LINUX_GAMELIFT=0");
                PublicDefinitions.Add("GAMESPARKS_SERVER=0");
            }
        }
		else
        {
            PublicDefinitions.Add("LINUX_GAMELIFT=0");
            PublicDefinitions.Add("GAMESPARKS_SERVER=0");
        }

        if ( (Target.Type == TargetRules.TargetType.Client) || (Target.Type == TargetRules.TargetType.Editor))
        {
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicDefinitions.Add("GAMELIFT_CLIENT=1");
                PublicDefinitions.Add("GAMESPARKS_CLIENT=1");
                if (Target.Type == TargetRules.TargetType.Client)
                {
                    bEnableExceptions = true;
                }
                PublicDependencyModuleNames.AddRange(new string[] { "AWSCore", "GameLiftClientSDK" });
            }
			else
            {
                PublicDefinitions.Add("GAMELIFT_CLIENT=0");
                PublicDefinitions.Add("GAMESPARKS_CLIENT=0");
            }
        }
        else
        {
            PublicDefinitions.Add("GAMELIFT_CLIENT=0");
            PublicDefinitions.Add("GAMESPARKS_CLIENT=0");
        }
    }
}

