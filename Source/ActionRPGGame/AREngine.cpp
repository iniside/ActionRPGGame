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
	
	FString ItemTags = "/Game/Blueprints/DataTables/InventoryTags.InventoryTags";
	TagsList.Add(ItemTags);

	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	TagsList.Add(Tags);

	FString DamageTags = "/Game/Blueprints/DamageTags.DamageTags";
	TagsList.Add(DamageTags);

	FString PawnTypesTags = "/Game/Blueprints/DataTables/PawnTypes.PawnTypes";
	TagsList.Add(PawnTypesTags);

	GameplayTagsModule.GetGameplayTagsManager().DestroyGameplayTagTree();
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	GameplayTagsModule.GetGameplayTagsManager().ConstructGameplayTagTree();
	Super::Init(InEngineLoop);

}
