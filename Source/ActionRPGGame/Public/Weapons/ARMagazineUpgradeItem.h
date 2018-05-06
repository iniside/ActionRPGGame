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
	/*
		An actual modifier value of this upgrade.
	*/
	UPROPERTY(EditAnywhere, SaveGame)
		float MagazineUpgradeValue;

	UPROPERTY(EditAnywhere)
		EGAAttributeMod ModType;
	/*
		Effect template
	*/
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UARMagazineUpgradeEffect> UpgradeEffect;
};
