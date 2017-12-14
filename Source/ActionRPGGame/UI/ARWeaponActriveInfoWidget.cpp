// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponActriveInfoWidget.h"
#include "ARPlayerController.h"

#include "ARAbilityBase.h"
#include "ARGunAttributes.h"
#include "../Weapons/ARWeaponManagerComponent.h"
#include "../Weapons/ARWeaponAbilityBase.h"

float UARWeaponActriveInfoWidget::GetActiveMagazineAmmo()
{
	if (!WeaponEquipement.IsValid())
		return 0;
	UGAAbilityBase* Ability = WeaponEquipement->GetCurrentWeapon();
	if (!Ability)
		return 0;

	if (UARGunAttributes* Attributes = Cast<UARGunAttributes>(Ability->Attributes))
	{
		return Attributes->Magazine.GetCurrentValue();
	}
	return 0;
}

float UARWeaponActriveInfoWidget::GetActiveMagazineAmmoNormalized()
{
	if (!WeaponEquipement.IsValid())
		return 0;
	UGAAbilityBase* Ability = WeaponEquipement->GetCurrentWeapon();
	if (!Ability)
		return 0;

	if (UARGunAttributes* Attributes = Cast<UARGunAttributes>(Ability->Attributes))
	{
		float CurrentValue = Attributes->Magazine.GetCurrentValue();
		FVector2D Input(0, Attributes->Magazine.GetFinalValue());
		return FMath::GetMappedRangeValueClamped(Input, FVector2D(0, 1), CurrentValue);
	}
	return 0;
}

float UARWeaponActriveInfoWidget::GetReloadTimeNormalized()
{
	if (!WeaponEquipement.IsValid())
		return 0;
	UGAAbilityBase* Ability = WeaponEquipement->GetCurrentWeapon();
	if (!Ability)
		return 0;

	return Ability->GetCooldownRemainingTimeNormalized();
}