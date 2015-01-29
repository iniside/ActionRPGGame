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
FEventReply UGISSlotBaseWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.
		if (!SlotInfo.ItemData)
			return Reply;

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply.NativeReply = Reply.NativeReply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	//Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton);
	return Reply;
}

void UGISSlotBaseWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{
	if (GISItemClass)
	{
		UWidget* superWidget = GetWidgetFromName(DropSlottName);
		UOverlay* overlay = Cast<UOverlay>(superWidget);
		//UGISItemBaseWidget* itemTemp = 
		UGISItemBaseWidget* ItemWidget = Cast<UGISItemBaseWidget>(overlay->GetChildAt(0)); //ConstructObject<UGISItemBaseWidget>(GISItemClass);
		if (ItemWidget && SlotInfo.CurrentInventoryComponent.IsValid())
		{
			ULocalPlayer* Player = SlotInfo.CurrentInventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
			ItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
			ItemWidget->Initialize();
			ItemWidget->LastSlotInfo = SlotInfo;
			ItemWidget->LastSlotInfo.ItemData = nullptr;
			ItemWidget->ItemData = SlotInfo.ItemData;
			ItemWidget->LastSlot = this;
		}

		UDragDropOperation* DragDropOp = ConstructObject<UDragDropOperation>(UDragDropOperation::StaticClass());
		if (DragDropOp)
		{
			DragDropOp->Payload = ItemWidget;
			DragDropOp->DefaultDragVisual = ItemWidget;

			Operation = DragDropOp;
		}
	}
}

bool UGISSlotBaseWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	if (Operation)
	{
		UGISItemBaseWidget* item = Cast<UGISItemBaseWidget>(Operation->Payload);
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