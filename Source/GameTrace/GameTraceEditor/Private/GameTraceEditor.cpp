// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameTraceEditor.h"



#include "GameTraceEditorPrivatePCH.h"


class FGameTraceEditor : public IGameTraceEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_GAME_MODULE(FGameTraceEditor, GameTraceEditor)


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



