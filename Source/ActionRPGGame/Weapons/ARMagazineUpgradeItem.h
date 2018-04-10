// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ARWeaponUpgradeItem.h"
#include "ARMagazineUpgradeItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARMagazineUpgradeItem : public UARWeaponUpgradeItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UARMagazineUpgradeEffect> UpgradeEffect;
};
