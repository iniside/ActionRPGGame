// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ActionRPGGameEditorTarget : TargetRules
{
	public ActionRPGGameEditorTarget(TargetInfo Target)
	{
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
        OutExtraModuleNames.Add("GameTrace");
        OutExtraModuleNames.Add("GameTraceEditor");
		OutExtraModuleNames.Add("GameInventorySystem");
		OutExtraModuleNames.Add("GameInventorySystemEditor");
		OutExtraModuleNames.Add("GameAttributes");
		OutExtraModuleNames.Add("GameAttributesEditor");
		OutExtraModuleNames.Add("GameEffectSystem");
		OutExtraModuleNames.Add("GameEffectSystemEditor");
		OutExtraModuleNames.Add("GameAbilities");
		OutExtraModuleNames.Add("GameAbilitiesEditor");
		OutExtraModuleNames.Add("GameFloatingCombatText");
		OutExtraModuleNames.Add("GameFloatingCombatTextEditor");
		OutExtraModuleNames.Add("GameSystem");
		OutExtraModuleNames.Add("GameSystemEditor");
		OutExtraModuleNames.Add("ActionRPGGame");
		OutExtraModuleNames.Add("ActionRPGGameEditor");
	}
}
