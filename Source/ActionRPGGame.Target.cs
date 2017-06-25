// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameTarget : TargetRules
{
	public ActionRPGGameTarget(TargetInfo Target) : base(Target)
	{
        //LinkType = TargetLinkType.Modular;
        //LinkType = TargetLinkType.Monolithic;
        LaunchModuleName = "ActionRPGGame";

        ExtraModuleNames.Add("TimeOfDay");
        ExtraModuleNames.Add("GameInterfaces");
        ExtraModuleNames.Add("GameTrace");
        ExtraModuleNames.Add("GameInventorySystem");
        ExtraModuleNames.Add("AbilityFramework");
        ExtraModuleNames.Add("GameSystem");
        ExtraModuleNames.Add("GameWidgets");
        ExtraModuleNames.Add("ActionRPGGame");
        //if (UEBuildConfiguration.bBuildEditor)
        //{
        //    ExtraModuleNames.Add("GameTraceEditor");
        //    ExtraModuleNames.Add("GameInventorySystemEditor");
        //    ExtraModuleNames.Add("AbilityFrameworkEditor");
        //    ExtraModuleNames.Add("GameWidgetsEditor");
        //    ExtraModuleNames.Add("GameSystemEditor");
        //    ExtraModuleNames.Add("ActionRPGGameEditor");
        //    ExtraModuleNames.Add("TimeOfDayEditor");
        //}

        Type = TargetType.Game;
	}
}
