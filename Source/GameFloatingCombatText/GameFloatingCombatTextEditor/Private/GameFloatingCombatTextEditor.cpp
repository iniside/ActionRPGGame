// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFloatingCombatTextEditor.h"



#include "GameFloatingCombatTextEditorPrivatePCH.h"


class FGameFloatingCombatTextEditor : public IGameFloatingCombatTextEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameFloatingCombatTextEditor, GameFloatingCombatTextEditor)


//void FGameAttributesEditor::StartupModule()
//{
//
//}
//
//
//void FGameAttributesEditor::ShutdownModule()
//{
//
//}



