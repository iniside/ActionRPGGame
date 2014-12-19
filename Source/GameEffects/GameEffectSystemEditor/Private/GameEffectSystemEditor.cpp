// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameEffectSystemEditor.h"



#include "GameEffectSystemEditorPrivatePCH.h"


class FGameEffectSystemEditor : public IGameEffectSystemEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_MODULE(FGameEffectSystemEditor, GameEffectSystemEditor)


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



