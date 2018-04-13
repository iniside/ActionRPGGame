// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemContainerWidget.h"
#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

void UIFItemContainerWidget::SetInventory(UIFInventoryComponent* InInventory)
{
	Inventory = InInventory;
}

void UIFItemContainerWidget::CreateInventory()
{
	APlayerController* PC = GetOwningPlayer();
	uint8 MaxSlots = Inventory->GetMaxSlots();

	for (uint8 Idx = 0; Idx < MaxSlots; Idx++)
	{
		UIFItemWidget* Widget = CreateWidget<UIFItemWidget>(PC, UIFItemWidget::StaticClass());
		const FIFItemData Slot = Inventory->GetSlot(Idx);

		Widget->OnSlotCreated(Slot.GetNetIndex(), Slot.GetLocalIndex());

		InventoryWidgets.Add(Widget);
	}

}