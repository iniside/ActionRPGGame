// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameSystem: ModuleRules
	{
		public GameSystem(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameSystem",
                    "GameSystem/Public",
                    "GameAbilities",
                    "GameAbilities/Public",
                    "GameAttributes",
                    "GameAttributes/Public",
                    "GameEffectSystem",
                    "GameEffectSystem/Public",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GameSystem/Private",
                    "GameAbilities/Private",
                    "GameAttributes/Private",
                    "GameEffectSystem/Private",
                    "GameInventorySystem/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				    "Core",
				    "CoreUObject",
				    "Engine",
                    "GameAbilities",
                    "GameAttributes",
                    "GameEffectSystem",
                    "GameInventorySystem"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}