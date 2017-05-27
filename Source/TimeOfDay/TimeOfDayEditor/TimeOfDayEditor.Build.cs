// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
using System.IO;
namespace UnrealBuildTool.Rules
{
	public class TimeOfDayEditor : ModuleRules
	{
		public TimeOfDayEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            var EngineDir = Path.GetFullPath(BuildConfiguration.RelativeEnginePath);
            PublicIncludePaths.AddRange(
				new string[] {
                    "TimeOfDay",
                    "TimeOfDay/Public",
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
                    "TimeOfDay/Private",
                     Path.Combine(EngineDir, @"Source/Editor/GraphEditor/Private"),
                     Path.Combine(EngineDir, @"Source/Editor/Kismet/Private"),
                     Path.Combine(EngineDir, @"Source/Editor/PropertyEditor/Private"),
                     Path.Combine(EngineDir, @"Source/Developer/AssetTools/Private")
					// ... add other private include paths required here ...
				}
				);

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
                    "GameplayTasks",
                    "GameplayTasksEditor",
                    "AbilityFramework",
                    "MovieScene",
                    "MovieSceneTools",
                    "MovieSceneTracks",
                    "Sequencer",
                    "ActorSequence",
                    "TimeOfDay",

					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                      "UnrealEd",
                    "AssetTools",
                    "PropertyEditor",
                    "MovieScene",
                    "MovieSceneTools",
                    "MovieSceneTracks",
                    "Sequencer",
                    "ActorSequence"
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