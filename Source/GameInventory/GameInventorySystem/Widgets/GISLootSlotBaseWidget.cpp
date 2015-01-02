// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "../GISItemData.h"
#include "../GISPickupActor.h"
#include "../GISInventoryBaseComponent.h"
#include "GISLootSlotBaseWidget.h"

UGISLootSlotBaseWidget::UGISLootSlotBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGISLootSlotBaseWidget::~UGISLootSlotBaseWidget()
{
	LootSlotInfo.SlotData = nullptr;
	LootSlotInfo.SlotComponent.Reset();
	LootSlotInfo.OwningPickupActor = nullptr;
}
UTexture2D* UGISLootSlotBaseWidget::GetIcon() const
{
	if (LootSlotInfo.SlotData)
	{
		return LootSlotInfo.SlotData->GetImage();
	}
	return nullptr;
}


FEventReply UGISLootSlotBaseWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (LootSlotInfo.SlotComponent.IsValid())
		{
			LootSlotInfo.SlotComponent->LootOneItem(LootSlotInfo.SlotIndex);
		}
	}
	return Reply;
}