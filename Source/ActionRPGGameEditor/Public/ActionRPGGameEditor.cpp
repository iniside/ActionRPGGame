// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "ActionRPGGameEditor.h"
#include "GameplayTagsModule.h"
#include "IGameEffectSystemEditor.h"
#include "IGameAttributesEditor.h"

class FActionRPGGameEditor : public IModuleInterface
{
	virtual void StartupModule() override
	{
		//because for some reason this module refuses to load automatically 0o.
		IGameAttributesEditor::Get();
		//and this too.
		IGameEffectSystemEditor::Get();
		IGameplayTagsModule::Get();
	}

	virtual void ShutdownModule() override
	{
	}

};

IMPLEMENT_GAME_MODULE(FActionRPGGameEditor, ActionRPGGameEditor);