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

#if LINUX_GAMELIFT
	//AcceptPlayerSession
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	FGameLiftGenericOutcome out = gameLiftSdkModule->AcceptPlayerSession(PlayerId);

	FString ErrorName = out.GetError().m_errorName;
	FString ErrorMessage = out.GetError().m_errorMessage;
#endif


	return Output;
}

void AARGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
#if LINUX_GAMELIFT
	if (const AARPlayerController* PC = Cast<AARPlayerController>(ExitingPlayer))
	{
		FString GLId = PC->GameLiftId;
		FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
		FGameLiftGenericOutcome out = gameLiftSdkModule->RemovePlayerSession(GLId);

		FString ErrorName = out.GetError().m_errorName;
		FString ErrorMessage = out.GetError().m_errorMessage;
	}
#endif

	Super::UnregisterPlayer(ExitingPlayer);

	/*AARGameMode* GM = Cast<AARGameMode>(GetOuter());
	int32 NumPlayer = GM->GetNumPlayers();
	if (NumPlayer <= 0)
	{
#if LINUX_GAMELIFT
		FString GLId = PC->GameLiftId;
		FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
		gameLiftSdkModule->TerminateGameSession();
#endif
	}*/
	
}