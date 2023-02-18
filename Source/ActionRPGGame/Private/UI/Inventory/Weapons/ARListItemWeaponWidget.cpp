// Fill out your copyright notice in the Description page of Project Settings.

#include "ARListItemWeaponWidget.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARItemView.h"


FReply UARListItemWeaponWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Handled = FReply::Unhandled();
	if (!Target.IsValid())
		return Handled;

	InventoryComponent->AddWeaponToSlot(Target->NetIndex, Target->LocalIndex, NetIndex, LocalIndex);
	Handled = FReply::Handled();

	return Handled;
}