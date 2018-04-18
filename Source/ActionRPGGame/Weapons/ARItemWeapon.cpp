// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeapon.h"
#include "Weapons/ARMagazineUpgradeItem.h"

void UARItemWeapon::AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade)
{
	MagazineModification = InMagazineUpgrade->GetClass()->GetPathName();
	MagazineEffect = InMagazineUpgrade->UpgradeEffect.LoadSynchronous();
	OnMagazineUpdateAdded();
}
void UARItemWeapon::OnMagazineUpdateAdded()
{

}

TSoftClassPtr<class UARMagazineUpgradeItem> UARItemWeapon::RemoveMagazineUpgrade()
{
	TSoftClassPtr<class UARMagazineUpgradeItem> Copy = MagazineModification;
	MagazineModification.Reset();
	return Copy;
}

void UARItemWeapon::OnItemAdded(uint8 LocalIndex)
{

}
void UARItemWeapon::OnItemRemoved(uint8 LocalIndex)
{

}