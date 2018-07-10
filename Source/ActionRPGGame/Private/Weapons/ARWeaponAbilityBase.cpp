// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponAbilityBase.h"
#include "Effects/GABlueprintLibrary.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARMagazineUpgradeItem.h"

#include "ARCharacter.h"
#include "ARWeaponBase.h"

void UARWeaponAbilityBase::SetAttributes(UGAAttributesBase* InAttributes)
{
	Attributes = InAttributes;
}
class UGAAttributesBase* UARWeaponAbilityBase::GetAttributes() const
{
	return Attributes;
}
class UGAAttributesBase* UARWeaponAbilityBase::GetAttributes()
{
	return Attributes;
}

void UARWeaponAbilityBase::OnAbilityInited()
{
	ResetDamageEffects();
}
void UARWeaponAbilityBase::OnAbilityInputReady()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		if (AARWeaponBase* Weapon = Cast<AARWeaponBase>(Character->GetEquipedMainWeapon()->GetChildActor()))
		{
			WeaponActor = Weapon;
		}
	}
}
void UARWeaponAbilityBase::OnAvatarReady()
{
	//AARWeaponBase* Weapon = Cast<AARWeaponBase>(AvatarActor);
	//Weapon->SetPawn(POwner);
	//Weapon->Equip();
}

void UARWeaponAbilityBase::ReplaceDamageEffects(const TArray<FAFPropertytHandle>& InEffects)
{

}

void UARWeaponAbilityBase::AddDamageEffects(const TArray<FAFPropertytHandle>& InEffects)
{

}

void UARWeaponAbilityBase::ResetDamageEffects()
{
	DamageEffects.Empty();
	DamageEffects.Append(DefaultDamageEffects);
	AppliedEffectHandles.Empty();
	AppliedEffectHandles.AddZeroed(DamageEffects.Num());
}

void UARWeaponAbilityBase::ApplyDamageEffect(UObject* Target, FAFFunctionModifier Modifier)
{
	for (int32 Idx = 0; Idx < DamageEffects.Num(); Idx++)
	{
		UGABlueprintLibrary::ApplyGameEffectToObject(DamageEffects[Idx]
			, Target
			, POwner
			, this
			, Modifier);
	}
}
void UARWeaponAbilityBase::ReloadWeapon()
{

}
void UARWeaponAbilityBase::AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade)
{
	AddMagazineUpgrade(InMagazineUpgrade->UpgradeEffect, InMagazineUpgrade->MagazineUpgradeValue);
}
void UARWeaponAbilityBase::AddMagazineUpgrade(TSubclassOf<class UARMagazineUpgradeEffect> InMagazineUpgrade, float UpgradeValue)
{
	MagazineUpgradeProperty = InMagazineUpgrade;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		AddUpgrade(MagazineUpgradeProperty, MagazineEffectHandle, MagazineSpecHandle, Character, UpgradeValue);
	}
}

void UARWeaponAbilityBase::RemoveMagazineUpgrade()
{
	FGAEffectMod Mod = MagazineSpecHandle.GetModifier();
	RemoveBonus(Mod.Attribute, MagazineEffectHandle, Mod.AttributeMod);
	UpgradeContainer.RemoveEffect(MagazineEffectHandle);

	MagazineUpgradeProperty.Reset();
	MagazineSpecHandle.Reset();
}

void UARWeaponAbilityBase::AddUpgrade(FAFPropertytHandle& PropertyHandle
		, FGAEffectHandle& EffectHandle
		, FAFEffectSpecHandle& SpecHandle
		, class AARCharacter* Character
		, float UpgradeValue)
{
	//UGABlueprintLibrary::CreateEffectSpec(SpecHandle, PropertyHandle, this, Character, Character);

	//EffectHandle = FGAEffectHandle::GenerateHandle();
	//SpecHandle.CalculateAttributeModifier(EffectHandle);
	//SpecHandle.OverrideAttributeModifier(UpgradeValue);

	//FGAEffect Effect(SpecHandle.GetPtr(), EffectHandle);

	//UpgradeContainer.ApplyEffect(EffectHandle, Effect);

	//FAFContextHandle Context = FAFContextHandle::Generate(&DefaultContext);

	//FGAEffectMod Mod = SpecHandle.GetModifier();
	//ModifyAttribute(Mod, EffectHandle, PropertyHandle.GetRef(), Context);

}