// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbilitiesComponent.h"
#include "../../GSPlayerController.h"

#include "GSAbilityBookItem.h"

#include "GSAbilityBookSlotWidget.h"

UGSAbilityBookSlotWidget::UGSAbilityBookSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FEventReply UGSAbilityBookSlotWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.
		if (!SlotInfo.AbilityClass)
			return Reply;

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply.NativeReply = Reply.NativeReply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	return Reply;
}

void UGSAbilityBookSlotWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{
	UWidget* superWidget = GetWidgetFromName(SlotName);
	UOverlay* overlay = Cast<UOverlay>(superWidget);
	//UGISItemBaseWidget* itemTemp = 
	UGSAbilityBookItem* ItemWidget = Cast<UGSAbilityBookItem>(overlay->GetChildAt(0)); //ConstructObject<UGISItemBaseWidget>(GISItemClass);
	if (ItemWidget && SlotInfo.AbilityClass)
	{
		ItemWidget->SlotInfo = SlotInfo;
	}

	UDragDropOperation* DragDropOp = ConstructObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		DragDropOp->Payload = ItemWidget;
		DragDropOp->DefaultDragVisual = ItemWidget;

		Operation = DragDropOp;
	}
}

bool UGSAbilityBookSlotWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	return false;
}