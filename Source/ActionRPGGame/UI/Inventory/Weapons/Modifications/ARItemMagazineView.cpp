// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemMagazineView.h"
#include "UI/Inventory/ARUIInventoryComponent.h"



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

