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
	//Function used to determine what happens if GameSparks connects or fails to (Needs to be UFUNCTION)
	UFUNCTION()
		void OnGameSparksAvailable(bool bAvailable);


	virtual void Init() override;
#if WITH_EDITOR
	
	/* Called to actually start the game when doing Play/Simulate In Editor */
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;

#endif
};
