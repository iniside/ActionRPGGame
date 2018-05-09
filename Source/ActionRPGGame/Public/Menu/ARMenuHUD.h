// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ARMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API AARMenuHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UARLoginScreenView> LoginScreenClass;
	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UARMainMenuView> MainMenuScreenClass;

	UPROPERTY(BlueprintReadOnly)
		class UARLoginScreenView* LoginScreen;
	
	UPROPERTY(BlueprintReadOnly)
		class UARMainMenuView* MainMenuScreen;

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void OnLoginSuccess();
	
};
