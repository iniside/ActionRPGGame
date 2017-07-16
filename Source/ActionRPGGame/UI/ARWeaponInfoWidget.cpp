// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponInfoWidget.h"


void UARWeaponInfoWidget::InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent)
{
	Super::InitializeWidget(InOwningComponent);
}

void UARWeaponInfoWidget::NativeAddAbility(const FGameplayTag& InAbilityTag)
{
	Super::NativeAddAbility(InAbilityTag);
}
void UARWeaponInfoWidget::NativeOnAbilityReady(const FGameplayTag& InAbilityTag)
{
	Super::NativeOnAbilityReady(InAbilityTag);

	WeaponAttributes = Ability->GetAttributesTyped<UARGunAttributes>();
}

float UARWeaponInfoWidget::GetMagazineAmmo()
{
	if (!WeaponAttributes)
		return 0;

	return WeaponAttributes->Magazine.GetCurrentValue();

}
float UARWeaponInfoWidget::GetMagazineAmmoNormalized()
{
	if (!WeaponAttributes)
		return 0;

	float CurrentValue = WeaponAttributes->Magazine.GetCurrentValue();
	FVector2D Input(0, WeaponAttributes->Magazine.GetFinalValue());

	return FMath::GetMappedRangeValueClamped(Input, FVector2D(0, 1), CurrentValue);

}