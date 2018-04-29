// Fill out your copyright notice in the Description page of Project Settings.

#include "ARListItemMagazineView.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARItemView.h"

FReply UARListItemMagazineView::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Handled = FReply::Unhandled();


	InventoryComponent->AddMagazineUpgrade(NetIndex, LocalIndex);
	Handled = FReply::Handled();

	return Handled;
}

