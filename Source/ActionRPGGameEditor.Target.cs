// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameEditorTarget : TargetRules
{
	public ActionRPGGameEditorTarget(TargetInfo Target)
	{
        //LinkType = TargetLinkType.Modular;
       // LinkType = TargetLinkType.Monolithic;
        Type = TargetType.Editor;
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
        OutExtraModuleNames.Add("GameInventorySystem");
        OutExtraModuleNames.Add("GameInventorySystemEditor");
        OutExtraModuleNames.Add("GameAbilities");
		OutExtraModuleNames.Add("GameAbilitiesEditor");
        OutExtraModuleNames.Add("GameWidgets");
        OutExtraModuleNames.Add("GameWidgetsEditor");
        OutExtraModuleNames.Add("GameWeapons");
        OutExtraModuleNames.Add("GameWeaponsEditor");
        OutExtraModuleNames.Add("GameSystem");
        OutExtraModuleNames.Add("GameSystemEditor");
        OutExtraModuleNames.Add("ActionRPGGame");
        OutExtraModuleNames.Add("ActionRPGGameEditor");
    }
}
