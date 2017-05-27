// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameServerTarget : TargetRules
{
    public ActionRPGGameServerTarget(TargetInfo Target) : base(Target)
    {
        LinkType = TargetLinkType.Modular;
        //LinkType = TargetLinkType.Monolithic;
        Type = TargetType.Server;
        bUsesSlate = false;
        ExtraModuleNames.Add("GameInventorySystem");
        ExtraModuleNames.Add("AbilityFramework");
        ExtraModuleNames.Add("GameSystem");
        ExtraModuleNames.Add("ActionRPGGame");
    }
}
