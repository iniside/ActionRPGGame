// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameTarget : TargetRules
{
	public ActionRPGGameTarget(TargetInfo Target)
	{
        //LinkType = TargetLinkType.Modular;
        //LinkType = TargetLinkType.Monolithic;
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
        OutExtraModuleNames.Add("TimeOfDay");
        OutExtraModuleNames.Add("GameInterfaces");
		OutExtraModuleNames.Add("GameTrace");
		OutExtraModuleNames.Add("GameInventorySystem");
		OutExtraModuleNames.Add("GameAbilities");
        OutExtraModuleNames.Add("GameSystem");
		OutExtraModuleNames.Add("GameWidgets");
        OutExtraModuleNames.Add("ActionRPGGame");
        if (UEBuildConfiguration.bBuildEditor)
        {
			OutExtraModuleNames.Add("GameTraceEditor");
			OutExtraModuleNames.Add("GameInventorySystemEditor");
			OutExtraModuleNames.Add("GameAbilitiesEditor");
			OutExtraModuleNames.Add("GameWidgetsEditor");
            OutExtraModuleNames.Add("GameSystemEditor");
            OutExtraModuleNames.Add("ActionRPGGameEditor");
        }
	}
}
