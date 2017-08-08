// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARWeaponInfoWidget.h"
#include "ARWeaponActriveInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponActriveInfoWidget : public UARWeaponInfoWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetActiveMagazineAmmo();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetActiveMagazineAmmoNormalized();

	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetReloadTimeNormalized();
	
};
