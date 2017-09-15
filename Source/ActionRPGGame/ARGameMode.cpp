// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ARGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Abilities/ARAbilityBase.h"
//#include "IpConnec"
//#include "OnlineSubsystemUtils/IpConnection.h"
#include "IPAddress.h"

AARGameMode::AARGameMode()
{
}
void AARGameMode::BeginPlay()
{
	Super::BeginPlay();
	//if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		if (UNetConnection* Conn = GetNetConnection())
		{
			UE_LOG(LogTemp, Warning, TEXT("Your message, %s \n"), *FString::FromInt(Conn->GetAddrAsInt()));
		}
	}
}