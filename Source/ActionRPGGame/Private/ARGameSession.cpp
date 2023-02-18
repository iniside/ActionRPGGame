// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameSession.h"
#include "ARGameInstance.h"
#include "ARGameMode.h"
#include "ARPlayerController.h"



FString AARGameSession::ApproveLogin(const FString& Options)
{
	FString Output = Super::ApproveLogin(Options);

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

	return Output;
}

void AARGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);
}