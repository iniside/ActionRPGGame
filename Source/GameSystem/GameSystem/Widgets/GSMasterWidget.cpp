// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widgets/GISItemBaseWidget.h"
#include "GISItemData.h"
#include "GISInventoryBaseComponent.h"
#include "GSMasterWidget.h"

UGSMasterWidget::UGSMasterWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UGSMasterWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	//handle dropping item from inventory to world.
	if (UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(Operation->Payload))
	{
		if (item->InventoryComponent)
		{
			item->InventoryComponent->DropItemFromInventory(item->LastSlotInfo);
			return true;
		}
	}

	return false;
}