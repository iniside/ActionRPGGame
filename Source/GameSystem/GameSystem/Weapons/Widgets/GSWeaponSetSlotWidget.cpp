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

FEventReply UGSWeaponSetSlotWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply.NativeReply = Reply.NativeReply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
	return Reply;
}

void UGSWeaponSetSlotWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{
	//if (ItemWidgetClass)
	//{
		//UGISItemBaseWidget* itemTemp = 
		UGSWeaponSetItemWidget* ItemWidgetDrag = ItemWidget;//ConstructObject<UGISItemBaseWidget>(GISItemClass);
		ItemWidgetDrag->WeaponSlot = WeaponSlot;
		UDragDropOperation* DragDropOp = ConstructObject<UDragDropOperation>(UDragDropOperation::StaticClass());
		if (DragDropOp)
		{
			DragDropOp->Payload = ItemWidgetDrag;
			DragDropOp->DefaultDragVisual = ItemWidgetDrag;

			Operation = DragDropOp;
		}
	//}
}

bool UGSWeaponSetSlotWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	if (Operation)
	{
		UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(Operation->Payload);
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
		UGSWeaponSetItemWidget* itemSet = Cast<UGSWeaponSetItemWidget>(Operation->Payload);
		if (itemSet)
		{
			WeaponEquipComp->AddWeaponToSet(itemSet->WeaponSlot.WeaponID, itemSet->WeaponSlot.WeaponTabID,
				WeaponSlot, itemSet->WeaponSlot);
			return true;
		}
	}
	return false;
}