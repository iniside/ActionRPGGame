// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "ActionRPGGameEditor.h"
#include "GameplayTagsModule.h"
#include "IGameAttributesEditor.h"
//#include "ISocketNameCustomization.h"
class FActionRPGGameEditor : public IModuleInterface
{
	virtual void StartupModule() override
	{
		//ISocketNameCustomization::Get();
		//because for some reason this module refuses to load automatically 0o.
	}

	virtual void ShutdownModule() override
	{
	}

};

IMPLEMENT_GAME_MODULE(FActionRPGGameEditor, ActionRPGGameEditor);