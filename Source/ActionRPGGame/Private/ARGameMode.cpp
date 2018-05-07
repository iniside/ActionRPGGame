// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ARGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Abilities/ARAbilityBase.h"
#include "ARGameInstance.h"
#include "SDraggableWindowWidget.h"
AARGameMode::AARGameMode()
{
}
void AARGameMode::BeginPlay()
{
	Super::BeginPlay();
	UARGameInstance* GSI = Cast<UARGameInstance>(GetGameInstance());

	//Set the OnAvailable delegate
	GSI->GetGSObject()->OnGameSparksAvailableDelegate.AddDynamic(GSI, &UARGameInstance::OnGameSparksAvailable);
	//Disconnected the module just incase it's connected (Refresh)
	GSI->GetGSObject()->Disconnect();
	//Connect module
	GSI->GetGSObject()->Connect("key", "secret");
}