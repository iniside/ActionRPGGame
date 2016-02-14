// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameAbilitiesEditor.h"



#include "GameAbilitiesEditorPrivatePCH.h"


class FGameAbilitiesEditor : public IGameAbilitiesEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameAbilitiesEditor, GameAbilitiesEditor);


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



