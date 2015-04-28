// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "../GISItemData.h"
#include "../GISPickupActor.h"
#include "../GISInventoryBaseComponent.h"
#include "GISItemBaseWidget.h"
#include "GISLootSlotBaseWidget.h"

UGISLootSlotBaseWidget::UGISLootSlotBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGISLootSlotBaseWidget::~UGISLootSlotBaseWidget()
{
	LootSlotInfo.SlotData = nullptr;
	//LootSlotInfo.SlotComponent.Reset();
	LootSlotInfo.OwningPickupActor = nullptr;
}

void UGISLootSlotBaseWidget::SetItemInfo(int32 ItemIndex)
{
	if (ItemWidget)
	{
		ItemWidget->ItemData = LootSlotInfo.SlotData;
	}
	
}
void UGISLootSlotBaseWidget::ResetSlot()
{
	ItemWidget->ItemData = nullptr;
	LootSlotInfo.Reset();
}

FReply UGISLootSlotBaseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (LootSlotInfo.SlotComponent.IsValid())
		{
			LootSlotInfo.SlotComponent->LootOneItem(LootSlotInfo.SlotIndex);
		}
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{

	}
	return Reply;
}

void UGISLootSlotBaseWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{

}

bool UGISLootSlotBaseWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}