// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameWeaponsEditor.h"



#include "IGameWeaponsEditor.h"


class FGameWeaponsEditor : public IGameWeaponsEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_MODULE(FGameWeaponsEditor, GameWeaponsEditor)


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



