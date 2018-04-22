// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeapon.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"
#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/Weapons/Modifications/ARItemMagazineView.h"

#include "Weapons/ARMagazineUpgradeItem.h"
#include "Weapons/ARWeaponInventoryComponent.h"


void UARItemWeapon::AddMagazineUpgrade(class UARMagazineUpgradeItem* InMagazineUpgrade)
{
	MagazineModification = InMagazineUpgrade->GetClass()->GetPathName();
	MagazineModificationObj = DuplicateObject<UARMagazineUpgradeItem>(InMagazineUpgrade, this);
	MagazineEffect = InMagazineUpgrade->UpgradeEffect.LoadSynchronous();
	OnMagazineUpdateAdded();
}
void UARItemWeapon::OnMagazineUpdateAdded()
{
	//should be called only fropm server.
	FARWeaponModInfo Info;
	Info.Icon = MagazineModificationObj->Icon->GetPathName();
	Info.UpgradeType = EARWeaponUpgradeType::Magazine;
	ClientOnMagazineAdded(Info);
}

TSoftClassPtr<class UARMagazineUpgradeItem> UARItemWeapon::RemoveMagazineUpgrade()
{
	TSoftClassPtr<class UARMagazineUpgradeItem> Copy = MagazineModification;
	MagazineModification.Reset();
	return Copy;
}

void UARItemWeapon::ClientOnMagazineAdded_Implementation(const FARWeaponModInfo& ModInfo)
{
	if (UARWeaponInventoryComponent* Inv = Cast<UARWeaponInventoryComponent>(GetOuter()))
	{
		if (AARCharacter* Character = Cast<AARCharacter>(Inv->GetOwner()))
		{
			if (AARPlayerController* PC = Cast<AARPlayerController>(Character->Controller))
			{
				if (AARHUD* HUD = Cast<AARHUD>(PC->GetHUD()))
				{
					UARUIInventoryComponent* UIInc = HUD->GetUIInventory();
					//not gonna work over network since we cant replicate uobject from uobject.
					UIInc->GetInventoryView()->MagazineUpgrade->OnItemChanged(0, 0, MagazineModificationObj);
				}
			}
		}
	}
}

void UARItemWeapon::OnItemAdded(uint8 LocalIndex)
{

}
void UARItemWeapon::OnItemRemoved(uint8 LocalIndex)
{

}