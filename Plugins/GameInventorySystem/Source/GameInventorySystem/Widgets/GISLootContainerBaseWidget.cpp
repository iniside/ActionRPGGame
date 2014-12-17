// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "../GISItemData.h"
#include "../GISPickupActor.h"

#include "GISLootSlotBaseWidget.h"
#include "GISLootContainerBaseWidget.h"

UGISLootContainerBaseWidget::UGISLootContainerBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UGISLootContainerBaseWidget::InitializeLootWidget()
{
	Slots.Empty();
	if (SlotClass)
	{
		for (const FGISLootSlotInfo& Item : ItemsInfos)
		{
			UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
			UGISLootSlotBaseWidget* ItemSlot = ConstructObject<UGISLootSlotBaseWidget>(SlotClass, Outer);
			if (ItemSlot)
			{
			//	OwningPickupActor->

				ULocalPlayer* Player = GetWorld()->GetFirstLocalPlayerFromController(); //temporary
				ItemSlot->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
				ItemSlot->Initialize();
				ItemSlot->LootSlotInfo = Item;
			}

			Slots.Add(ItemSlot);
		}
		PostLootWidgetInitialized();
	}
}