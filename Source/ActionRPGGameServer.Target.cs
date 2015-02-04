// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameServerTarget : TargetRules
{
    public ActionRPGGameServerTarget(TargetInfo Target)
	{
        Type = TargetType.Server;
        bUsesSlate = false;
	}

	//
	// TargetRules interface.
	//
    
	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.Add("GameInventorySystem");
		OutExtraModuleNames.Add("GameAttributes");
		OutExtraModuleNames.Add("GameAbilities");
		OutExtraModuleNames.Add("GameSystem");
		OutExtraModuleNames.Add("GameFloatingCombatText");
        OutExtraModuleNames.Add("ActionRPGGame");
    }

    public override bool ShouldCompileMonolithic(UnrealTargetPlatform InPlatform, UnrealTargetConfiguration InConfiguration)
    {
        return false;
    }
}
