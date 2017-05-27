// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameEditorTarget : TargetRules
{
	public ActionRPGGameEditorTarget(TargetInfo Target) : base (Target)
	{
        //LinkType = TargetLinkType.Modular;
       // LinkType = TargetLinkType.Monolithic;
        Type = TargetType.Editor;
        ExtraModuleNames.Add("TimeOfDay");
        ExtraModuleNames.Add("GameInterfaces");
        ExtraModuleNames.Add("GameInventorySystem");
        ExtraModuleNames.Add("GameInventorySystemEditor");
        ExtraModuleNames.Add("AbilityFramework");
        ExtraModuleNames.Add("AbilityFrameworkEditor");
        ExtraModuleNames.Add("GameWidgets");
        ExtraModuleNames.Add("GameWidgetsEditor");
        ExtraModuleNames.Add("GameWeapons");
        ExtraModuleNames.Add("GameWeaponsEditor");
        ExtraModuleNames.Add("GameSystem");
        ExtraModuleNames.Add("GameSystemEditor");
        ExtraModuleNames.Add("ActionRPGGame");
        ExtraModuleNames.Add("ActionRPGGameEditor");
        ExtraModuleNames.Add("TimeOfDayEditor");
    }
}
