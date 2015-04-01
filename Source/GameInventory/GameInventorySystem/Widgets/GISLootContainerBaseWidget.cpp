// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "../GISItemData.h"
#include "../GISPickupActor.h"
#include "../GISInventoryBaseComponent.h"
#include "GISItemBaseWidget.h"
#include "GISLootSlotBaseWidget.h"
#include "GISLootContainerBaseWidget.h"

UGISLootContainerBaseWidget::UGISLootContainerBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UGISLootContainerBaseWidget::InitializeLootWidget(const FGISLootConfiguration& ConfigIn, class UGISInventoryBaseComponent* OwningCompIn,
	APlayerController* PCOwnerIn)
{
	OwningComp = OwningCompIn;
	PCOwner = PCOwnerIn;
	Config = ConfigIn;
	OwningComp->OnLootingStart.AddUObject(this, &UGISLootContainerBaseWidget::UpdateLootWidget);
	Slots.Empty();
	if (Config.IsValid())
	{
		int32 MaxSlots = Config.MaxLootingSlots;
		for (int32 SlotIndex = 0; SlotIndex < MaxSlots; SlotIndex++)
		{
			UGISLootSlotBaseWidget* ItemSlot = CreateWidget<UGISLootSlotBaseWidget>(PCOwner, Config.LootSlotClass);
			if (ItemSlot)
			{
				UGISItemBaseWidget* item = CreateWidget<UGISItemBaseWidget>(PCOwner, Config.LootItemClass);
				ItemSlot->ItemWidget = item;

				UWidget* superWidget = ItemSlot->GetWidgetFromName(Config.LootItemSlotName);
				UOverlay* overlay = Cast<UOverlay>(superWidget);
				overlay->AddChild(item);
			}
			Slots.Add(ItemSlot);
		}
	}
	PostLootWidgetInitialized();
}

void UGISLootContainerBaseWidget::UpdateLootWidget()
{
	int32 SlotNum = Slots.Num();
	TArray<FGISLootSlotInfo>& ItemRef = OwningComp->LootFromPickup.Loot;
	for (int32 ItemIndex = 0; ItemIndex < SlotNum; ItemIndex++)
	{
		Slots[ItemIndex]->ResetSlot();
	}
	int32 ItemNum = OwningComp->LootFromPickup.Loot.Num();
	
	for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
	{
		Slots[ItemIndex]->LootSlotInfo = ItemRef[ItemIndex];
		Slots[ItemIndex]->SetItemInfo(ItemIndex);// = ItemRef[ItemIndex];
	}
}