// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemContainerWidget.h"
#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

void UIFItemContainerWidget::SetInventory(UIFInventoryComponent* InInventory)
{
	Inventory = InInventory;
	Inventory->GetOnInventoryRead().AddUObject(this, &UIFItemContainerWidget::CreateInventory);
}

void UIFItemContainerWidget::CreateInventory()
{
	APlayerController* PC = GetOwningPlayer();
	uint8 MaxSlots = Inventory->GetMaxSlots();

	for (uint8 Idx = 0; Idx < MaxSlots; Idx++)
	{
		UIFItemWidget* Widget = CreateWidget<UIFItemWidget>(PC, ItemClass);
		Widget->Inventory = Inventory;
		
		const FIFItemData& Slot = Inventory->GetSlot(Idx);
		
		FIFOnItemChangedEvent Event = FIFOnItemChangedEvent::CreateUObject(Widget, &UIFItemWidget::OnItemChanged);
		const_cast<FIFItemData&>(Slot).SetOnItemChanged(Event);

		Widget->OnSlotCreated(Slot.GetNetIndex(), Slot.GetLocalIndex());

		InventoryWidgets.Add(Widget);
	}
	NativeOnInventoryCreated();
}

void UIFItemContainerWidget::NativeOnInventoryCreated()
{
	BP_OnInventoryCreated();
}