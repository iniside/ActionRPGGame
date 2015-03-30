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

	//IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
	//
	//TArray<FString> TagsList;

	//FString Tags = "/Game/Blueprints/SampleTags.SampleTags";
	//TagsList.Add(Tags);
	//
	//FString DamageTags = "/Game/Blueprints/DamageTags.DamageTags";
	//TagsList.Add(DamageTags);

	//FString ItemTags = "/Game/Blueprints/DataTables/InventoryTags.InventoryTags";
	//TagsList.Add(ItemTags);
	//
	//FString PawnTypesTags = "/Game/Blueprints/DataTables/PawnTypes.PawnTypes";
	//TagsList.Add(PawnTypesTags);

	//GameplayTagsModule.GetGameplayTagsManager().DestroyGameplayTagTree();
	//GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
	//GameplayTagsModule.GetGameplayTagsManager().ConstructGameplayTagTree();
}
