// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class GameSystemEditor : ModuleRules
	{
		public GameSystemEditor(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
                    "GameSystem",
                    "GameSystem/Public",
                    "GameSystemEditor",
                    "GameSystemEditor/Public",
                    "GameWeapons",
                    "GameWeapons/Public",
                    "GameWeaponsEditor",
                    "GameWeaponsEditor/Public",
                    "GameAbilities",
                    "GameAbilities/Public",
                    "GameAbilitiesEditor",
                    "GameAbilitiesEditor/Public",
                    "GameAttributes",
                    "GameAttributes/Public",
                    "GameAttributesEditor",
                    "GameAttributesEditor/Public",
                    "GameEffectSystem",
                    "GameEffectSystem/Public",
                    "GameEffectSystemEditor",
                    "GameEffectSystemEditor/Public",
                    "GameInventorySystem",
                    "GameInventorySystem/Public",
                    "GameInventorySystemEditor",
                    "GameInventorySystemEditor/Public",
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
                    "GameSystem/Private",
					"GameSystemEditor/Private",
                    "GameWeapons/Private",
                    "GameWeaponsEditor/Private",
                    "GameAbilities/Private",
                    "GameAbilitiesEditor/Private",
                    "GameAttributes/Private",
                    "GameAttributesEditor/Private",
                    "GameEffectSystem/Private",
                    "GameEffectSystemEditor/Private",
                    "GameInventorySystem/Private",
                    "GameInventorySystemEditor/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "Engine",
                    "CoreUObject",
                    "UnrealEd",
                    "BlueprintGraph",
                    "GraphEditor",
                    "PropertyEditor",
                    "SlateCore",
                    "Slate",
                    "EditorStyle",
                    "Kismet",
                    "KismetCompiler",
                    "AssetTools",
                    "MainFrame",
                    "InputCore",
                    "GameSystem",
                    "GameTrace",
                    "GameTraceEditor",
                    "GameAbilities",
                    "GameWeapons",
                    "GameWeaponsEditor",
                    "GameAttributes",
                    "GameEffectSystem",
                    "GameInventorySystem",
                    "GameAbilitiesEditor",
                    "GameAttributesEditor",
                    "GameEffectSystemEditor",
                    "GameInventorySystemEditor"
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