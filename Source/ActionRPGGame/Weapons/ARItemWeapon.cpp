// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeapon.h"
#include "Weapons/ARMagazineUpgradeItem.h"

void UARItemWeapon::AddMagazineUpgrade(const TSoftClassPtr<class UARMagazineUpgradeItem>& InMagazineUpgrade)
{
	TSubclassOf<UARMagazineUpgradeItem> clas = InMagazineUpgrade.LoadSynchronous();
	MagazineEffect = clas->GetDefaultObject<UARMagazineUpgradeItem>()->UpgradeEffect.LoadSynchronous();
	OnMagazineUpdateAdded();
}
void UARItemWeapon::OnMagazineUpdateAdded()
{

}
void UARItemWeapon::OnItemAdded(uint8 LocalIndex)
{

}
void UARItemWeapon::OnItemRemoved(uint8 LocalIndex)
{

}