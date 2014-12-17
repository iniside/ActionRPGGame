// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameInventorySystemEditor.h"
#include "GameInventorySystemEditorPrivatePCH.h"


class FGameInventorySystemEditor : public IGameInventorySystemEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FGameInventorySystemEditor, GameInventorySystemEditor)


void FGameInventorySystemEditor::StartupModule()
{

}


void FGameInventorySystemEditor::ShutdownModule()
{

}



