// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameServerTarget : TargetRules
{
    public ActionRPGGameServerTarget(TargetInfo Target)
	{
        LinkType = TargetLinkType.Modular;
        //LinkType = TargetLinkType.Monolithic;
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
		OutExtraModuleNames.Add("GameAbilities");
		OutExtraModuleNames.Add("GameSystem");
		OutExtraModuleNames.Add("GameWidgets");
        OutExtraModuleNames.Add("ActionRPGGame");
    }
}
