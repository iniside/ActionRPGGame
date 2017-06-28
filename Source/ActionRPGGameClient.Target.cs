// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameClientTarget : TargetRules
{
    public ActionRPGGameClientTarget(TargetInfo Target) : base(Target)
    {
        //LinkType = TargetLinkType.Modular;
        //LinkType = TargetLinkType.Monolithic;
        Type = TargetType.Client;
        LaunchModuleName = "ActionRPGGame";

        ExtraModuleNames.Add("TimeOfDay");
        ExtraModuleNames.Add("GameInterfaces");
        ExtraModuleNames.Add("GameTrace");
        ExtraModuleNames.Add("GameInventorySystem");
        ExtraModuleNames.Add("AbilityFramework");
        ExtraModuleNames.Add("GameSystem");
        ExtraModuleNames.Add("GameWidgets");
        ExtraModuleNames.Add("ActionRPGGame");
    }
}
