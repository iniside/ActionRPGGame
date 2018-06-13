// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameEditorTarget : TargetRules
{
	public ActionRPGGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("ActionRPGGame");
        ExtraModuleNames.Add("ActionRPGGameEditor");
    }
}
