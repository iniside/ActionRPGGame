// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Abilities/ARAbilityInfoWidget.h"
#include "ARGunAttributes.h"
#include "ARWeaponInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponInfoWidget : public UARAbilityInfoWidget
{
	GENERATED_BODY()
protected:
	class UARGunAttributes* WeaponAttributes;
public:
	virtual void InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent) override;	
	virtual void NativeAddAbility(const FGameplayTag& InAbilityTag) override;
	virtual void NativeOnAbilityReady(const FGameplayTag& InAbilityTag) override;
	
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetMagazineAmmo();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Weapon")
		float GetMagazineAmmoNormalized();
};
