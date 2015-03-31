// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameInventorySystem : ModuleRules
	{
		public GameInventorySystem(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameInventorySystem/Widgets",
                    "GameInterfaces"
                
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"GameInventorySystem/Private"
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				    "Core",
				    "CoreUObject",
				    "Engine",
                    "InputCore",
                    "UMG",
                    "Slate",
                    "SlateCore",
                    "GameplayTags",
                    "GameInterfaces"
                    //
					// ... add other public dependencies that you statically link with here ...
				}
				);
            if (Target.Type == TargetRules.TargetType.Editor)
            {
                PublicDependencyModuleNames.AddRange(new string[] { "UnrealEd", "PropertyEditor" });
            }
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