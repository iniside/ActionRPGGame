// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ARWeaponUpgradeItem.h"
#include "ARMagazineUpgradeItem.generated.h"


USTRUCT()
struct FARMagazineUpgradeItemData : public FARItemBaseData
{
	GENERATED_BODY()
	/*
		replace with instance, to make procedural generation easier. 
	*/
	UPROPERTY(EditAnywhere)
		TSoftClassPtr<class UARMagazineUpgradeEffect> UpgradeEffect;
};

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARMagazineUpgradeItem : public UARWeaponUpgradeItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FARMagazineUpgradeItemData Data;
	UPROPERTY(EditAnywhere)
		TSoftClassPtr<class UARMagazineUpgradeEffect> UpgradeEffect;
};
