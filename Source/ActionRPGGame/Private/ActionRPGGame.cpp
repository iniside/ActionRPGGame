// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "IGameEffectSystem.h"
#include "IGameAttributes.h"
#include "GameplayTagsModule.h"

class FActionRPGGame : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		//because they don't want to load when editor starts ;/
		IGameAttributes::Get();
		IGameEffectSystem::Get();
		IGameplayTagsModule::Get();
		//IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
		//FString Tags = "/Game/Blueprints/SampleTags.SampleTags";

		////UDataTable* justDeleteMe = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
		////just for test

		//UDataTable* TagTable = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);

		//TArray<FString> TagsList;
		//TagsList.Add(Tags);
		//GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	}

	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FActionRPGGame, ActionRPGGame, "ActionRPGGame");
 