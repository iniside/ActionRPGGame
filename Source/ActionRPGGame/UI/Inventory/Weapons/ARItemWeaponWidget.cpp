// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeaponWidget.h"

#include "IFInventoryComponent.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARWeaponInventoryComponent.h"

#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/Weapons/ARWeaponContainerWidget.h"
#include "UI/Inventory/Weapons/ARListItemWeaponWidget.h"

void UARItemWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(GetOwningPlayer())
	{
		if (AARHUD* HUD = Cast<AARHUD>(GetOwningPlayer()->GetHUD()))
		{
			InventoryComponent = HUD->GetUIInventory();
		}
	}
}


void UARItemWeaponWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	InventoryComponent->ShowWeaponsForSlot(this);
}
void UARItemWeaponWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

}
FReply UARItemWeaponWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Handled = FReply::Unhandled();

	InventoryComponent->UnequipWeaponFromSlot(NetIndex, LocalIndex);
	Handled = FReply::Handled();

	return Handled;
}
