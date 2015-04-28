// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../../Items/GSItemWeaponInfo.h"
#include "Widgets/GISItemBaseWidget.h"
#include "GISGlobalTypes.h"
#include "GSWeaponSetItemWidget.h"

#include "../GSWeaponEquipmentComponent.h"

#include "GSWeaponSetSlotWidget.h"

UGSWeaponSetSlotWidget::UGSWeaponSetSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FReply UGSWeaponSetSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply = Reply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
	return Reply;
}

void UGSWeaponSetSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{
	//if (ItemWidgetClass)
	//{
		//UGISItemBaseWidget* itemTemp = 
		UGSWeaponSetItemWidget* ItemWidgetDrag = ItemWidget;//ConstructObject<UGISItemBaseWidget>(GISItemClass);
		ItemWidgetDrag->WeaponSlot = WeaponSlot;
		UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
		if (DragDropOp)
		{
			DragDropOp->Payload = ItemWidgetDrag;
			DragDropOp->DefaultDragVisual = ItemWidgetDrag;

			InOperation = DragDropOp;
		}
	//}
}

bool UGSWeaponSetSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation)
	{
		UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(InOperation->Payload);
		if (item)
		{
			UGSWeaponEquipmentComponent* eq = Cast<UGSWeaponEquipmentComponent>(item->LastSlotInfo.CurrentInventoryComponent.Get());
			if (!eq)
				return false;

			item->LastSlotInfo.SlotIndex;
			item->LastSlotInfo.SlotTabIndex;

			WeaponEquipComp->AddWeaponToSet(item->LastSlotInfo.SlotIndex, item->LastSlotInfo.SlotTabIndex,
				WeaponSlot);
			WeaponSlot.WeaponID = item->LastSlotInfo.SlotIndex;
			return true;
		}
		UGSWeaponSetItemWidget* itemSet = Cast<UGSWeaponSetItemWidget>(InOperation->Payload);
		if (itemSet)
		{
			WeaponEquipComp->AddWeaponToSet(itemSet->WeaponSlot.WeaponID, itemSet->WeaponSlot.WeaponTabID,
				WeaponSlot, itemSet->WeaponSlot);
			return true;
		}
	}
	return false;
}