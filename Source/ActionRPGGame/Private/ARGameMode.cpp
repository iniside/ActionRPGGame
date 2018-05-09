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
}