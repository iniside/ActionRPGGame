// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ARGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Abilities/ARAbilityBase.h"
#include "ARGameInstance.h"
#include "ARPlayerController.h"



AARGameMode::AARGameMode()
{

}
void AARGameMode::BeginPlay()
{
	Super::BeginPlay();

}

FString AARGameMode::InitNewPlayer(APlayerController* NewPlayerController
	, const FUniqueNetIdRepl& UniqueId
	, const FString& Options
	, const FString& Portal)
{
	FString ReturnString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	TArray<FString> OutParams;
	Options.ParseIntoArray(OutParams, TEXT("?"));

	TArray<FString> OutPlayerId;
	FString Left;
	FString PlayerId;
	for (FString& str : OutParams)
	{
		if (str.Split("PlayerId=", &Left, &PlayerId))
		{
			break;
		}
	}

	if (AARPlayerController* PC = Cast<AARPlayerController>(NewPlayerController))
	{
		PC->GameLiftId = PlayerId;
	}

	return ReturnString;
}