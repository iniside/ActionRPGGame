// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTags.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
		class UARUIWeaponEquipment* UIWeaponEquipment;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
		class UARWeaponManagerComponent* WeaponManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Input")
		FGameplayTag InputNextWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Input")
		FGameplayTag AbilitytNextWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Input")
		FGameplayTag InputPreviousWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Input")
		FGameplayTag AbilitytPreviousWeapon;

public:
	AARPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void SetPawn(APawn* InPawn) override;
	void SetupInputComponent();

	void InputSwitchAbilitySet();

	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Weapons")
		void NextWeapon();
	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Weapons")
		void PreviousWeapon();

	void OnInputAbilityReady(FGameplayTag InAbilityTag, FGameplayTag InInputTag);
};
