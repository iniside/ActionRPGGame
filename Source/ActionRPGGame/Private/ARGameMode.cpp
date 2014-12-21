// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "GameplayTagsModule.h"
#include "ARGameMode.h"
#include "ARCharacter.h"

AARGameMode::AARGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AARGameMode::BeginPlay()
{
	Super::BeginPlay();

	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	FString DamageTags = "/Game/Blueprints/DamageTags.DamageTags";
	//UDataTable* justDeleteMe = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	//just for test
	TagTable = LoadObject<UDataTable>(NULL, *Tags, NULL, LOAD_None, NULL);
	DamageTable = LoadObject<UDataTable>(NULL, *DamageTags, NULL, LOAD_None, NULL);

	IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();

	TArray<FString> TagsList;
	TagsList.Add(Tags);
	TagsList.Add(DamageTags);
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
}