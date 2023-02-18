// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System.IO;
namespace UnrealBuildTool.Rules
{
	public class AbilityFrameworkEditor : ModuleRules
	{
		public AbilityFrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);

			PrivateIncludePaths.AddRange(
				new string[] {
                    "AbilityFrameworkEditor/Private",
                     Path.Combine(EngineDir, @"Source/Editor/GraphEditor/Private"),
                     Path.Combine(EngineDir, @"Source/Editor/Kismet/Private"),
                     Path.Combine(EngineDir, @"Source/Editor/PropertyEditor/Private"),
                     Path.Combine(EngineDir, @"Source/Developer/AssetTools/Private"),
					// ... add other private include paths required here ...
				}
				);
            PrivateIncludePathModuleNames.AddRange(
                new string[] {
                    "UnrealEd",
                    "AssetTools",
                    "PropertyEditor",
                    "MovieScene",
                    "MovieSceneTools",
                    "MovieSceneTracks",
                    "Sequencer",
                    "ActorSequence"
                });
            PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "Engine",
                    "CoreUObject",
                    "AssetTools",
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
                    "AbilityFramework",
                    "MovieScene",
                    "MovieSceneTools",
                    "MovieSceneTracks",
                    "Sequencer",
                    "ActorSequence"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "Core",
                    "Engine",
                    "CoreUObject",
                    "AssetTools",
                    "UnrealEd",
                    "PropertyEditor",
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