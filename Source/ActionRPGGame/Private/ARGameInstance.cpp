// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"

#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Abilities/GAAbilityBase.h"
#include "Effects/AFCueActor.h"

#if WITH_AGONES
#include "IAgones.h"
#endif

UARGameInstance::UARGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UARGameInstance::OnGameSparksAvailable(bool bAvailable)
{

	OnConnectedToGameSparks.Broadcast();
}

void UARGameInstance::AttemptLogin(const FString& UserName, const FString& Password)
{

	
}

void UARGameInstance::RegisterNewPlayer(const FString& UserName, const FString& DisplayName, const FString& Password)
{

}

void UARGameInstance::Init()
{
	Super::Init();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	//AssetRegistryModule.Get().OnFilesLoaded().AddUObject(this, &UARUIAbilityManagerComponent::FinishedLoadinFiles);
	TArray< FString > ContentPaths;
	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(ContentPaths);
	AssetRegistry.ScanPathsSynchronous(ContentPaths);

	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		Manager->ScanPathsForPrimaryAssets(FPrimaryAssetType("Ability"), ContentPaths, UGAAbilityBase::StaticClass(), true);
		Manager->ScanPathsForPrimaryAssets(FPrimaryAssetType("EffectCue"), ContentPaths, AAFCueActor::StaticClass(), true);
	}

#if WITH_AGONES
	if (IAgones::Get().AgonesSDK->Connect())
	{
		grpc::Status statuc = IAgones::Get().AgonesSDK->Ready();
		FTimerDelegate HealthCheckDel = FTimerDelegate::CreateUObject(this, &UARGameInstance::HealthCheck);
		TimerManager->SetTimer(HealthCheckHandle, HealthCheckDel, 1, true, 1);
	}
#endif
}
#if WITH_EDITOR

/* Called to actually start the game when doing Play/Simulate In Editor */
FGameInstancePIEResult UARGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	return Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
}

#endif

void UARGameInstance::ConnectToHub()
{

}


void UARGameInstance::ConnectToWorld()
{

}


void UARGameInstance::TestGSRequest()
{
}


void UARGameInstance::HealthCheck()
{
#if WITH_AGONES
	IAgones::Get().AgonesSDK->Health();
#endif
}
