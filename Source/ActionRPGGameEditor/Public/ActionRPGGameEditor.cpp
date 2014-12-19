// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGameEditor.h"

class FActionRPGGameEditor : public IModuleInterface
{
	virtual void StartupModule() override
	{

	}

	virtual void ShutdownModule() override
	{
	}

};

IMPLEMENT_GAME_MODULE(FActionRPGGameEditor, ActionRPGGameEditor);