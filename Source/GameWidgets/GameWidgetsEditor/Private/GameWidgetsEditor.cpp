// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameWidgetsEditor.h"



#include "GameWidgetsEditorPrivatePCH.h"


class FGameWidgetsEditor : public IGameWidgetsEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameWidgetsEditor, GameWidgetsEditor)


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



