// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"

#include "GameplayTagsModule.h"
#include "AREngine.h"

UAREngine::UAREngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAREngine::Init(IEngineLoop* InEngineLoop)
{
	IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
	TArray<FString> TagsList;
	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	FString DamageTags = "/Game/Blueprints/DamageTags.DamageTags";
	//UDataTable* justDeleteMe = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	//just for test
	UDataTable* TagTable = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	FString ItemTags = "/Game/Blueprints/DataTables/InventoryTags.InventoryTags";
	TagsList.Add(ItemTags);
	TagsList.Add(Tags);
	TagsList.Add(DamageTags);
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	GameplayTagsModule.GetGameplayTagsManager().ConstructGameplayTagTree();
	Super::Init(InEngineLoop);

}
