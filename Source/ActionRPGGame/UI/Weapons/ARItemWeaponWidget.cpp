// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeaponWidget.h"

#include "IFInventoryComponent.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARWeaponInventoryComponent.h"

#include "UI/Inventory/ARWeaponContainerWidget.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"

void UARItemWeaponWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	TArray<uint8> Items = Inventory->GetLocalItemIdxs(UARItemWeapon::StaticClass());

	UARItemWeapon* CurrentItem = WeaponInventory->GetItem<UARItemWeapon>(LocalIndex);

	if (CurrentItem)
	{
		InventoryWidget->SetWeaponName(CurrentItem->GetName());
	}
	InventoryWidget->UpdateItemList(Items);
}
void UARItemWeaponWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

}