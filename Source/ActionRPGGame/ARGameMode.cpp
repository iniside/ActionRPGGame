// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ARGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Abilities/ARAbilityBase.h"
AARGameMode::AARGameMode()
{
}
void AARGameMode::BeginPlay()
{
	Super::BeginPlay();
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	//IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	////AssetRegistryModule.Get().OnFilesLoaded().AddUObject(this, &UARUIAbilityManagerComponent::FinishedLoadinFiles);
	//TArray< FString > ContentPaths;
	//TArray<FString> RootPaths;
	//FPackageName::QueryRootContentPaths(ContentPaths);
	//AssetRegistry.ScanPathsSynchronous(ContentPaths);
	//if (UAssetManager* Manager = UAssetManager::GetIfValid())
	//{
	//	Manager->StartBulkScanning();
	//	Manager->ScanPathsForPrimaryAssets(FPrimaryAssetType("Ability"), ContentPaths, UGAAbilityBase::StaticClass(), true);
	//	Manager->ScanPathsForPrimaryAssets(FPrimaryAssetType("Ability"), ContentPaths, UARAbilityBase::StaticClass(), true);
	//	//Manager->ScanPrimaryAssetTypesFromConfig();
	//}
}