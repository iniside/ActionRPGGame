// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <GameSparks/GS.h>
#include <GameSparks/generated/GSResponses.h>
#include <GameSparks/generated/GSRequests.h>
#include "GameSparksModule.h"
#include "GameSparks/Private/GameSparksObject.h"
#include "GameSparks/Private/GSMessageListenersObject.h"

#if LINUX_GAMELIFT
#include "GameLiftServerSDK.h"
#include "aws/gamelift/server/model/GameProperty.h"
#include "aws/gamelift/server/model/GameSession.h"
#include "aws/gamelift/server/model/Player.h"
#include "aws/gamelift/server/model/PlayerSession.h"
#include "aws/gamelift/server/model/PlayerSessionStatus.h"
#endif


#if GAMELIFT_CLIENT
#include "aws/core/client/AWSError.h"
#include "aws/core/http/HttpRequest.h"
#include "aws/gamelift/GameLiftClient.h"
#include "aws/gamelift/GameLiftErrors.h"
#include "aws/core/utils/Outcome.h"
#include "aws/gamelift/model/DescribeGameSessionDetailsRequest.h"
#include "aws/gamelift/model/CreateGameSessionResult.h"
#include "aws/gamelift/model/CreateGameSessionRequest.h"
#include "aws/gamelift/model/CreatePlayerSessionRequest.h"
#include "aws/gamelift/model/SearchGameSessionsRequest.h"
#include "aws/gamelift/model/SearchGameSessionsResult.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#endif

#include "ARGameInstance.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAROnConnectedToGS);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FARLoginAttemptEvent);

/**
 * 
 */
UCLASS(Config=ARGame)
class ACTIONRPGGAME_API UARGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GSObjects")
		UGameSparksObject* GSObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GSObjects")
		UGSMessageListenersObject* GSMessageListener;

	UPROPERTY(Config)
		FString GSKey;
	UPROPERTY(Config)
		FString GSSecret;
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
		FAROnConnectedToGS OnConnectedToGameSparks;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
		FARLoginAttemptEvent OnLoginSuccess;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
		FARLoginAttemptEvent OnLoginFailed;

#if GAMELIFT_CLIENT	
	Aws::GameLift::GameLiftClient* GameLiftClient;
	FString AccessKey;
	FString Secret;
	FString Region;
	int32 LocalPort;
	bool bIsUsingGameLiftLocal;
#endif

#if WITH_EDITOR
	//editor hax so we don't attempt to connect twice.
	bool bConnected;
#endif
public:
	UARGameInstance(const FObjectInitializer& ObjectInitializer);

	inline UGameSparksObject* GetGSObject()
	{
		return GSObject;
	}

	inline UGSMessageListenersObject* GetGSMessageListener()
	{
		return GSMessageListener;
	}


	void AttemptLogin(const FString& UserName, const FString& Password);
	void RegisterNewPlayer(const FString& UserName, const FString& DisplayName, const FString& Password);
	//Function used to determine what happens if GameSparks connects or fails to (Needs to be UFUNCTION)
	UFUNCTION()
		void OnGameSparksAvailable(bool bAvailable);


	virtual void Init() override;
#if WITH_EDITOR
	
	/* Called to actually start the game when doing Play/Simulate In Editor */
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;

#endif

#if LINUX_GAMELIFT
	void InitGameLift();
	void OnGameSessionStarted(Aws::GameLift::Server::Model::GameSession InGameSession);
#endif

	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void ConnectToHub();
	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void ConnectToWorld();

#if GAMELIFT_CLIENT
	void OnSessionsSearched(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::SearchGameSessionsRequest& Request, const Aws::GameLift::Model::SearchGameSessionsOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context);
	void OnSessionCreated(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::CreateGameSessionRequest& Request, const Aws::GameLift::Model::CreateGameSessionOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context);
	void OnCreatePlayerSession(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::CreatePlayerSessionRequest& Request, const Aws::GameLift::Model::CreatePlayerSessionOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context);
#endif

	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void TestGSRequest();

	void OnScriptMessageReceived(GameSparks::Api::Responses::LogEventResponse response);

protected:
	TSharedPtr<FJsonObject> GetGameLiftResponseFromGS(const FString& Field, GameSparks::Api::Responses::LogEventResponse response);
};
