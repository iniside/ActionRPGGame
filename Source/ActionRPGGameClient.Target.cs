// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameClientTarget : TargetRules
{
    public ActionRPGGameClientTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Client;
        LaunchModuleName = "ActionRPGGame";
        ExtraModuleNames.Add("ActionRPGGame");
    }
}
