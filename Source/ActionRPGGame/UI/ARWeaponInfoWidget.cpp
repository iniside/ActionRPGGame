// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponInfoWidget.h"
#include "ARPlayerController.h"
#include "ARAbilityBase.h"
#include "ARGunAttributes.h"
#include "../Weapons/ARWeaponAbilityBase.h"
#include "../Weapons/ARWeaponManagerComponent.h"
void UARWeaponInfoWidget::NativePreConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		WeaponEquipement = MyPC->WeaponManager;
	}
Super::NativePreConstruct();
}
void UARWeaponInfoWidget::NativeConstruct()
{
	UARWeaponManagerComponent* WMC = nullptr;
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		WeaponEquipement = MyPC->WeaponManager;
		WMC = MyPC->WeaponManager;
	}
	if (WeaponSlotTag.IsValid())
	{
	//	FAROnWeaponReady Delegate = FAROnWeaponReady::CreateUObject(this, &UARWeaponInfoWidget::OnWeaponChanged);
	//	WMC->AddOnWeaponChangedEvent(WeaponSlotTag, Delegate);
	}
	Super::NativeConstruct();
}

float UARWeaponInfoWidget::GetMagazineAmmo()
{
	UGAAbilityBase* Ability = WeaponEquipement->GetAbility(SlotIndex, EAMSlot::Slot001);
	if (!Ability)
		return 0;

	if (UARGunAttributes* Attributes = Cast<UARGunAttributes>(Ability->Attributes))
	{
		return Attributes->Magazine.GetCurrentValue();
	}
	return 0;
}
float UARWeaponInfoWidget::GetMagazineAmmoNormalized()
{
	UGAAbilityBase* Ability = WeaponEquipement->GetAbility(SlotIndex, EAMSlot::Slot001);
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

void UARWeaponInfoWidget::OnWeaponChanged(class UARWeaponAbilityBase* InAbility)
{
	
}