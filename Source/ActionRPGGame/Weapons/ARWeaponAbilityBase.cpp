// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponAbilityBase.h"
#include "Effects/GABlueprintLibrary.h"
#include "ARCharacter.h"
#include "ARWeaponBase.h"

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
		UGABlueprintLibrary::ApplyEffectToObject(DamageEffects[Idx]
			, AppliedEffectHandles[Idx]
			, Target
			, POwner
			, this
			, Modifier);
	}
}
void UARWeaponAbilityBase::ReloadWeapon()
{

}