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
                    "GameWeapons",
                    "GameWeapons/Public",
                    "GameAttributes",
                    "GameAttributes/Public",
					"GameAttributes/Effects",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameWidgets",
                    "GameWidgets/Public",
                    "GameTrace",
                    "GameTrace/Public",
                    "GameInterfaces",
                    "GameInterfaces/Public"
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GameSystem/Private",
                    "GameWeapons/Private",
                    "GameAbilities/Private",
                    "GameAttributes/Private",
                    "GameInventorySystem/Private",
                    "GameWidgets/Private",
                    "GameTrace/Private",
                    "GameInterfaces/Private"
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				    "Core",
                    "InputCore",
				    "CoreUObject",
				    "Engine",
                    "UMG",
                    "Slate",
                    "SlateCore",
                    "GameInterfaces",
                    "GameTrace",
                    "GameAbilities",
                    "GameWeapons",
                    "GameAttributes",
                    "GameInventorySystem",
                    "GameWidgets",
                    "GameplayTags"
                   // 
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