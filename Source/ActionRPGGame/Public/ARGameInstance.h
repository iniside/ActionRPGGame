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

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GSObjects")
		UGameSparksObject* GSObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GSObjects")
		UGSMessageListenersObject* GSMessageListener;

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

	//Function used to determine what happens if GameSparks connects or fails to (Needs to be UFUNCTION)
	UFUNCTION()
		void OnGameSparksAvailable(bool bAvailable);

	//Add the response function to the Authentication request
	static void AuthenticationRequest_Response(GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse&);
};
