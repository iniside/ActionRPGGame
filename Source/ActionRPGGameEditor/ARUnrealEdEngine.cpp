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
	Super::Init(InEngineLoop);

	IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
	FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	FString DamageTags = "/Game/Blueprints/DamageTags.DamageTags";
	FString ItemTags = "/Game/Blueprints/DataTables/InventoryTags.InventoryTags";
	TArray<FString> TagsList;
	TagsList.Add(Tags);
	TagsList.Add(DamageTags);
	TagsList.Add(ItemTags);
	GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	GameplayTagsModule.GetGameplayTagsManager().ConstructGameplayTagTree();
}
