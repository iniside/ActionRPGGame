// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GISInventoryBaseComponent.h"
#include "GSInventorySlotWidget.h"

UGSInventorySlotWidget::UGSInventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//FEventReply UGSInventorySlotWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
//{
//	FEventReply Reply;
//	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
//	{
//		//no item no mouse interaction for dragging and dropping.
//		if (!SlotInfo.ItemData)
//			return Reply;
//
//		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
//		if (DetectingDrag.IsValid())
//		{
//			Reply.NativeReply = Reply.NativeReply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
//		}
//	}
//
//	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
//	return Reply;
//}

//void UGSInventorySlotWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
//{
//		UWidget* superWidget = GetWidgetFromName(DropSlottName);
//		UOverlay* overlay = Cast<UOverlay>(superWidget);
//		//UGISItemBaseWidget* itemTemp = 
//		UGISItemBaseWidget* ItemWidget = Cast<UGISItemBaseWidget>(overlay->GetChildAt(0)); //ConstructObject<UGISItemBaseWidget>(GISItemClass);
//		if (ItemWidget && SlotInfo.CurrentInventoryComponent.IsValid())
//		{
//			ItemWidget->LastSlotInfo = SlotInfo;
//			ItemWidget->LastSlotInfo.ItemData = nullptr;
//			ItemWidget->ItemData = SlotInfo.ItemData;
//			ItemWidget->LastSlot = this;
//		}
//
//		UDragDropOperation* DragDropOp = ConstructObject<UDragDropOperation>(UDragDropOperation::StaticClass());
//		if (DragDropOp)
//		{
//			DragDropOp->Payload = ItemWidget;
//			DragDropOp->DefaultDragVisual = ItemWidget;
//
//			Operation = DragDropOp;
//		}
//}

bool UGSInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool returnValue = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//if (Operation)
	//{
	//	UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(Operation->Payload);
	//	if (item)
	//	{
	//		SlotInfo.CurrentInventoryComponent->AddItemOnSlot(SlotInfo, item->LastSlotInfo);
	//		returnValue = true;
	//		//item->LastSlot->SlotInfo.ItemData = nullptr;
	//		//item->LastSlot.Reset();
	//		return returnValue;
	//	}
	//}
	return returnValue;
}