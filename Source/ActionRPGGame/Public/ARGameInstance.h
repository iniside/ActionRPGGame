// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

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
public:
	UARGameInstance(const FObjectInitializer& ObjectInitializer);

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

	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void ConnectToHub();
	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void ConnectToWorld();


	UFUNCTION(BlueprintCallable, Category = "GameLift|Test")
		void TestGSRequest();

protected:
#if WITH_AGONES
	FTimerHandle HealthCheckHandle;
#endif
	UFUNCTION()
		void HealthCheck();

};
