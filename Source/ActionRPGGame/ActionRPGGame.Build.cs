// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionRPGGame : ModuleRules
{
    public ActionRPGGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDefinitions.Add("BOOST_SYSTEM_NOEXCEPT");
        PublicDefinitions.Add("GPR_FORBID_UNREACHABLE_CODE=1");
        PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
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
            "Json",
            "JsonUtilities",
            "GameplayTags",
            "AbilityFramework",
            "AssetRegistry",
            "OrionAnimation",
            "ActorSequence",
            "JsonUObject",
            "InventoryFramework",
            "OnlineSubsystem"
        });

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "SourceControl", "Matinee", "PropertyEditor", "ShaderCore", "AbilityFrameworkEditor" });
            PrivateDependencyModuleNames.AddRange(new string[] { "AbilityFrameworkEditor" });
        }
        PublicDependencyModuleNames.AddRange(new string[] { "GRPC" });
        PublicDefinitions.Add("GPR_GCOV=1"); //hax
        if (Target.Type == TargetRules.TargetType.Server)
        {
            if (Target.Platform == UnrealTargetPlatform.Linux)
            {
                PublicDefinitions.Add("WITH_AGONES=1");
                PublicDefinitions.Add("ENABLE_GRPC=1"); //hack
                PublicDependencyModuleNames.AddRange(new string[] { "Agones" });

            }
            else
            {
                PublicDefinitions.Add("WITH_AGONES=0");
                PublicDefinitions.Add("ENABLE_GRPC=0"); //hack
            }
        }
		else
        {
            PublicDefinitions.Add("WITH_AGONES=0");
            PublicDefinitions.Add("ENABLE_GRPC=0"); //hack
        }

        if ( (Target.Type == TargetRules.TargetType.Client) || (Target.Type == TargetRules.TargetType.Editor))
        {
            PublicDependencyModuleNames.AddRange(new string[] {
				"Slate",
				"SlateCore",
				"UMG",
                "InventoryFrameworkUI",
                "DraggableWindow"
            });

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                if (Target.Type == TargetRules.TargetType.Client)
                {
                    bEnableExceptions = true;
                }
            }
        }
    }
}

