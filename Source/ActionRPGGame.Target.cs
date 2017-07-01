// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameTarget : TargetRules
{
	public ActionRPGGameTarget(TargetInfo Target) : base(Target)
	{
                LaunchModuleName = "ActionRPGGame";
                ExtraModuleNames.Add("ActionRPGGame");
                Type = TargetType.Game;
	}
}
