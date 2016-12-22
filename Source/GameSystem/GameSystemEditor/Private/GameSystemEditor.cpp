// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameSystemEditor.h"



#include "IGameSystemEditor.h"


class FGameSystemEditor : public IGameSystemEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameSystemEditor, GameSystemEditor)


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



