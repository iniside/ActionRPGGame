// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "ARItemWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ACTIONRPGGAME_API UARItemWeapon : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Ability")
		TSoftClassPtr<class UGAAbilityBase> Ability;
	
	UPROPERTY(EditAnywhere, Category = "Visual")
		TSoftClassPtr<class AARWeaponBase> Weapon;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector HolsteredPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator HolsteredRotation;

	UPROPERTY(EditAnywhere, Category = "Transforms")
		FVector EquipedPosition;
	UPROPERTY(EditAnywhere, Category = "Transforms")
		FRotator EquipedRotation;

	/* Wrap it later into it's own item. */
	UPROPERTY(EditAnywhere, Category = "Upgrades")
		TSoftClassPtr<class UARMagazineUpgradeEffect> MagazineUpgrade;

	void AddMagazineUpgrade(const TSoftClassPtr<class UARMagazineUpgradeEffect>& InMagazineUpgrade)
	{
		MagazineUpgrade = InMagazineUpgrade;
	}
	void OnMagazineUpdateAdded();
};
