// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemMagazineView.h"
#include "Components/Image.h"
#include "ARCharacter.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "Weapons/ARMagazineUpgradeItem.h"

void UARItemMagazineView::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer())
	{
		if (AARCharacter* Character = Cast<AARCharacter>(GetOwningPlayer()->GetPawn()))
		{
			Character->WeaponInventory->GetOnUpgradeInstalled().AddDynamic(this, &UARItemMagazineView::OnMagazineUpgradeInstalled);
			Character->WeaponInventory->GetOnUpgradeRemoved().AddDynamic(this, &UARItemMagazineView::OnMagazineUpgradeRemoved);
		}
	}
}

FReply UARItemMagazineView::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	InventoryComponent->ShowUpgradesForWeapon(this);
	return FReply::Unhandled();
}

FReply UARItemMagazineView::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Handled = FReply::Unhandled();

	InventoryComponent->RemoveMagazineUpgrade();
	Handled = FReply::Handled();

	return Handled;
}

void UARItemMagazineView::OnMagazineUpgradeInstalled(class UARItemWeapon* Weapon
	, class UARWeaponUpgradeItem* Upgrade
	, int8 WeaponIndex)
{
	if (UARMagazineUpgradeItem* Magazine = Cast<UARMagazineUpgradeItem>(Upgrade))
	{
		Icon->SetBrushFromTexture(Upgrade->Icon);
	}
}

void UARItemMagazineView::OnMagazineUpgradeRemoved(class UARItemWeapon* Weapon
	, class UARWeaponUpgradeItem* Upgrade
	, int8 WeaponIndex)
{
	if (UARMagazineUpgradeItem* Magazine = Cast<UARMagazineUpgradeItem>(Upgrade))
	{
		Icon->SetBrushFromTexture(nullptr);
	}
}