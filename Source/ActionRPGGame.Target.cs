// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameTarget : TargetRules
{
	public ActionRPGGameTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
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
        OutExtraModuleNames.Add("GameTrace");
		OutExtraModuleNames.Add("GameInventorySystem");
		OutExtraModuleNames.Add("GameAttributes");
		OutExtraModuleNames.Add("GameEffectSystem");
		OutExtraModuleNames.Add("GameAbilities");
		OutExtraModuleNames.Add("GameSystem");
		OutExtraModuleNames.Add("GameFloatingCombatText");
        OutExtraModuleNames.Add("ActionRPGGame");
        if (UEBuildConfiguration.bBuildEditor)
        {
            OutExtraModuleNames.Add("GameTraceEditor");
			OutExtraModuleNames.Add("GameInventorySystemEditor");
			OutExtraModuleNames.Add("GameAttributesEditor");
			OutExtraModuleNames.Add("GameEffectSystemEditor");
			OutExtraModuleNames.Add("GameAbilitiesEditor");
			OutExtraModuleNames.Add("GameFloatingCombatTextEditor");
			OutExtraModuleNames.Add("GameSystemEditor");
            OutExtraModuleNames.Add("ActionRPGGameEditor");
        }
	}
}
