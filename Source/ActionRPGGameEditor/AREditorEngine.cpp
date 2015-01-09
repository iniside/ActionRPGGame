// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGameEditor.h"
#include "GameplayTagsModule.h"
#include "AREditorEngine.h"

UAREditorEngine::UAREditorEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAREditorEngine::Init(IEngineLoop* InEngineLoop)
{
	IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
	
	////UDataTable* justDeleteMe = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	TArray<FString> TagsList;

	FString ItemTags = "/Game/Blueprints/DataTables/InventoryTags.InventoryTags";
	TagsList.Add(ItemTags);

	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	TagsList.Add(Tags);
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	Super::Init(InEngineLoop);
}
