// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARGameMode.generated.h"

UCLASS(minimalapi)
class AARGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AARGameMode();

	virtual void BeginPlay() override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
};



