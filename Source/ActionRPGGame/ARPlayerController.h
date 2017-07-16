// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ARPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API AARPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
		class UARUIComponent* UIComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
		class UARUIAbilityManagerComponent* UIAbilityManagerComponent;

public:
	AARPlayerController(const FObjectInitializer& ObjectInitializer);

	void SetupInputComponent();

	void InputSwitchAbilitySet();
};
