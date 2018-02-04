// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/ARWeaponBaseWidget.h"
#include "AMTypes.h"
#include "ARWeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponSlotWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		EAMGroup WeaponSlot;
	
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
		float GetCurrentReloadTime() const;
	UFUNCTION(BlueprintPure, Category = "Weapon")
		float GetCurrentReloadTimeNormalized() const;
	UFUNCTION(BlueprintPure, Category = "Weapon")
		float GetMagazineAmmo() const;
	UFUNCTION(BlueprintPure, Category = "Weapon")
		float GetRemaningAmmo() const;
	
};
