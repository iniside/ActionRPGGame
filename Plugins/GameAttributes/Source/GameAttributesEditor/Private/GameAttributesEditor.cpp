// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameAttributesEditor.h"
#include "GameAttributesEditorPrivatePCH.h"


class FGameAttributesEditor : public IGameAttributesEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FGameAttributesEditor, GameAttributesEditor)


void FGameAttributesEditor::StartupModule()
{

}


void FGameAttributesEditor::ShutdownModule()
{

}



