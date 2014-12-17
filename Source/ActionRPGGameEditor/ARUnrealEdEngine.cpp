// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGameEditor.h"

#include "GameplayTagsModule.h"
#include "ARUnrealEdEngine.h"

UARUnrealEdEngine::UARUnrealEdEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UARUnrealEdEngine::Init(IEngineLoop* InEngineLoop)
{
	IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	//UDataTable* justDeleteMe = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	//just for test
	UDataTable* TagTable = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);

	TArray<FString> TagsList;
	TagsList.Add(Tags);
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);

	Super::Init(InEngineLoop);

}
