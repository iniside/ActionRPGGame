// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class ActionRPGGameServerTarget : TargetRules
{
    public ActionRPGGameServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        //bUsesSlate = false;
        LaunchModuleName = "ActionRPGGame";
        ExtraModuleNames.Add("ActionRPGGame");
    }
}
