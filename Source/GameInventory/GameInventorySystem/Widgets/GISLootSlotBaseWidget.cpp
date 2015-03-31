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
UTexture2D* UGISLootSlotBaseWidget::GetIcon() const
{
	if (LootSlotInfo.OwningPickupActor)
	{
		if (LootSlotInfo.OwningPickupActor->ItemToLoot[LootSlotInfo.SlotIndex])
			return LootSlotInfo.OwningPickupActor->ItemToLoot[LootSlotInfo.SlotIndex]->GetImage();
	}
	return nullptr;
}

void UGISLootSlotBaseWidget::SetItemInfo(int32 ItemIndex)
{
	if (ItemWidget)
	{
		if (LootSlotInfo.OwningPickupActor)
			ItemWidget->ItemData = LootSlotInfo.OwningPickupActor->ItemToLoot[ItemIndex];
	}
}
void UGISLootSlotBaseWidget::ResetSlot()
{
	ItemWidget->ItemData = nullptr;
}
FEventReply UGISLootSlotBaseWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (LootSlotInfo.SlotComponent.IsValid())
		{
			LootSlotInfo.SlotComponent->LootOneItem(LootSlotInfo.SlotIndex);
		}
	}
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{

	}
	return Reply;
}

void UGISLootSlotBaseWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{

}

bool UGISLootSlotBaseWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	return false;
}