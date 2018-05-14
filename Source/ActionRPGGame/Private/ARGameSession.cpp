// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameSession.h"




FString AARGameSession::ApproveLogin(const FString& Options)
{
	FString Output = Super::ApproveLogin(Options);

	TArray<FString> OutParams;
	Options.ParseIntoArray(OutParams, TEXT("?"));
	
	int32 Idx = Options.Find("PlayerId=");
	int32 KeyIdx = Idx + 9;

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