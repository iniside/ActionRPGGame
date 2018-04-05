// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class AbilityFramework : ModuleRules
	{
		public AbilityFramework(ReadOnlyTargetRules Target) : base(Target)
        {
			PublicIncludePaths.AddRange(
				new string[] {
                    "AbilityFramework",
                    "AbilityFramework/Abilities",
                    "AbilityFramework/Attributes",
                    "AbilityFramework/Effects",
                    "AbilityFramework/Effects/ApplicationRequirement",
                    "AbilityFramework/Effects/CustomApplications",
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
                    "AbilityFramework/LatentActions",
                    "AbilityFramework/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				    "Core",
				    "CoreUObject",
				    "Engine",
                    "GameplayTags",
                    "InputCore",
                    "UMG",
                    "Slate",
                    "SlateCore",
                    "GameplayTasks",
                    "AIModule",
                    "MovieScene",
                    "MovieSceneTracks",
                    "AssetRegistry"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "ActorSequence",
                    "TimeManagement"
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
                     
					// ... add any modules that your module loads dynamically here ...
				}
				);
            if (Target.Type == TargetRules.TargetType.Editor)
            {
                PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "PropertyEditor" });
            }
        }
	}
}