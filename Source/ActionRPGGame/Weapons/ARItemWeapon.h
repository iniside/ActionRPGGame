// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effects/GAGameEffect.h"
#include "ARItemBase.h"
#include "GameplayTags.h"
#include "ARItemWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ACTIONRPGGAME_API UARItemWeapon : public UARItemBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Ability")
		TSoftClassPtr<class UARWeaponAbilityBase> Ability;
	
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

	UPROPERTY(EditAnywhere, Category = "Ability")
		UARWeaponAbilityBase* AbilityInstance;

	UPROPERTY(EditAnywhere, Category = "Ability")
		TSoftClassPtr<class UARMagazineUpgradeItem> MagazineModification;


	FAFPropertytHandle MagazineEffect;
	FGAEffectHandle MagazineEffectHandle;

	void AddMagazineUpgrade(const TSoftClassPtr<class UARMagazineUpgradeItem>& InMagazineUpgrade);
	void OnMagazineUpdateAdded();


	virtual void OnItemAdded(uint8 LocalIndex) override;
	virtual void OnItemRemoved(uint8 LocalIndex) override;
};
