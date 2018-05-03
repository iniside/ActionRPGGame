// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeapon.h"

#include "Effects/GABlueprintLibrary.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"
#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/Weapons/Modifications/ARItemMagazineView.h"

#include "Weapons/ARMagazineUpgradeItem.h"
#include "Weapons/ARWeaponInventoryComponent.h"
#include "Weapons/ARMagazineUpgradeEffect.h"

void UARItemWeapon::AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade)
{
	UARWeaponInventoryComponent* WeaponComponent = Cast<UARWeaponInventoryComponent>(GetOuter());

	if (!WeaponComponent)
		return;

	AARCharacter* Character = Cast<AARCharacter>(WeaponComponent->GetOwner());
	if (!Character)
		return;

	MagazineModification = DuplicateObject<UARMagazineUpgradeItem>(InMagazineUpgrade, this);

	//this part only on server.
	if (Character->Role >= ENetRole::ROLE_Authority)
	{
		TSubclassOf<UARMagazineUpgradeEffect> SpecClass = InMagazineUpgrade->UpgradeEffect.LoadSynchronous();
		MagazineUpgradeValue = InMagazineUpgrade->MagazineUpgradeValue;
		MagazineUpgradeProperty = SpecClass;

		AddUpgrade(MagazineUpgradeProperty, MagazineEffectHandle, MagazineSpecHandle, Character, MagazineUpgradeValue);
	}
	OnMagazineUpdateAdded();
}
void UARItemWeapon::OnMagazineUpdateAdded()
{
	//should be called only from server.
	FARWeaponModInfo Info;
	Info.Icon = MagazineModification->Icon->GetPathName();
	Info.UpgradeType = EARWeaponUpgradeType::Magazine;
}

void UARItemWeapon::AddUpgrade(FAFPropertytHandle& PropertyHandle
	, FGAEffectHandle& EffectHandle
	, FAFEffectSpecHandle& SpecHandle
	, class AARCharacter* Character
	, float UpgradeValue)
{
	//causer MagazineUpgrade ?
	UGABlueprintLibrary::CreateEffectSpec(SpecHandle, PropertyHandle, this, Character, Character);

	EffectHandle = FGAEffectHandle::GenerateHandle();
	SpecHandle.CalculateAttributeModifier(EffectHandle);
	SpecHandle.OverrideAttributeModifier(UpgradeValue);

	FGAEffect Effect(SpecHandle.GetPtr(), EffectHandle);

	UpgradeContainer.ApplyEffect(EffectHandle, Effect);

	FGAEffectMod Mod = SpecHandle.GetModifier();
	AbilityInstance->ModifyAttribute(Mod, EffectHandle, PropertyHandle.GetRef());
}

UARMagazineUpgradeItem* UARItemWeapon::RemoveMagazineUpgrade()
{
	FGAEffectMod Mod = MagazineSpecHandle.GetModifier();
	AbilityInstance->RemoveBonus(Mod.Attribute, MagazineEffectHandle, Mod.AttributeMod);
	UpgradeContainer.RemoveEffect(MagazineEffectHandle);
	
	MagazineUpgradeProperty.Reset();
	MagazineSpecHandle.Reset();

	return MagazineModification;
}


void UARItemWeapon::OnItemAdded(uint8 LocalIndex)
{

}
void UARItemWeapon::OnItemRemoved(uint8 LocalIndex)
{

}

void UARItemWeapon::PostItemLoad()
{

}