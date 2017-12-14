// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARUMGWidgetBase.h"
#include "ARGunAttributes.h"
#include "GameplayTags.h"
#include "../Weapons/ARWeaponsTypes.h"
#include "AMTypes.h"
#include "ARWeaponInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponInfoWidget : public UARUMGWidgetBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAMGroup SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FGameplayTag WeaponSlotTag;

	TWeakObjectPtr<class UARWeaponAbilityBase> Weapon;

	TWeakObjectPtr<class UARWeaponManagerComponent> WeaponEquipement;
public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetMagazineAmmo();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetMagazineAmmoNormalized();

	UFUNCTION()
		void OnWeaponChanged(class UARWeaponAbilityBase* InAbility);
};
