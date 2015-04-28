// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "../GISInventoryBaseComponent.h"
#include "GISItemBaseWidget.h"
#include "../GISGlobalTypes.h"
#include "GISSlotBaseWidget.h"

#include "GISSlotBaseWidget.h"

UGISSlotBaseWidget::UGISSlotBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
\
FReply UGISSlotBaseWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.
		if (!SlotInfo.ItemData)
			return Reply;

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply = Reply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
	return Reply;
}

void UGISSlotBaseWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{
		UWidget* superWidget = GetWidgetFromName(DropSlottName);
		UOverlay* overlay = Cast<UOverlay>(superWidget);
		//UGISItemBaseWidget* itemTemp = 
		UGISItemBaseWidget* ItemWidget = Cast<UGISItemBaseWidget>(overlay->GetChildAt(0)); //ConstructObject<UGISItemBaseWidget>(GISItemClass);
		if (ItemWidget && SlotInfo.CurrentInventoryComponent.IsValid())
		{
			ItemWidget->LastSlotInfo = SlotInfo;
			ItemWidget->LastSlotInfo.ItemData = nullptr;
			ItemWidget->ItemData = SlotInfo.ItemData;
			ItemWidget->LastSlot = this;
		}

		UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
		if (DragDropOp)
		{
			DragDropOp->Payload = ItemWidget;
			DragDropOp->DefaultDragVisual = ItemWidget;

			InOperation = DragDropOp;
		}
}

bool UGISSlotBaseWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation)
	{
		UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(InOperation->Payload);
		if (item)
		{
			SlotInfo.CurrentInventoryComponent->AddItemOnSlot(SlotInfo, item->LastSlotInfo);
			//item->LastSlot->SlotInfo.ItemData = nullptr;
			//item->LastSlot.Reset();
			return true;
		}
	}
	return false;
}