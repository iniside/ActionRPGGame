// Fill out your copyright notice in the Description page of Project Settings.

#include "ARInventoryScreenWidget.h"

#include "Components/TextBlock.h"

#include "UI/Inventory/Weapons/ARItemWeaponWidget.h"

#include "ARPlayerController.h"

void UARInventoryScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WeaponSlots.Add(RightWeaponWidget);
	WeaponSlots.Add(LeftWeaponWidget);
	WeaponSlots.Add(SideWeaponWidget);
	WeaponSlots.Add(BottomBackWeaponWidget);
}

void UARInventoryScreenWidget::SetWeaponName(const FString& Name)
{
	SelectedWeapon->SetText(FText::FromString(Name));
}

void UARInventoryScreenWidget::OnWeaponAdded(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item)
{
	WeaponSlots[NetIndex]->OnSlotCreated(NetIndex, LocalIndex, Item);
}
void UARInventoryScreenWidget::OnWeaponUpdated(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item)
{
	WeaponSlots[NetIndex]->OnItemChanged(NetIndex, LocalIndex, Item);
}
void UARInventoryScreenWidget::OnWeaponRemoved(uint8 NetIndex, uint8 LocalIndex, class UIFItemBase* Item)
{
	WeaponSlots[NetIndex]->OnItemRemoved(NetIndex, LocalIndex, Item);
}