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

FReply UGSAbilityBookSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//no item no mouse interaction for dragging and dropping.
		if (!SlotInfo.AbilityClass)
			return Reply;

		TSharedPtr<SWidget> DetectingDrag = this->GetCachedWidget();
		if (DetectingDrag.IsValid())
		{
			Reply = Reply.DetectDrag(DetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}

	return Reply;
}

void UGSAbilityBookSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{
	UWidget* superWidget = GetWidgetFromName(SlotName);
	UOverlay* overlay = Cast<UOverlay>(superWidget);
	//UGISItemBaseWidget* itemTemp = 
	UGSAbilityBookItem* ItemWidget = Cast<UGSAbilityBookItem>(overlay->GetChildAt(0)); //ConstructObject<UGISItemBaseWidget>(GISItemClass);
	if (ItemWidget && SlotInfo.AbilityClass)
	{
		ItemWidget->SlotInfo = SlotInfo;
	}

	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		DragDropOp->Payload = ItemWidget;
		DragDropOp->DefaultDragVisual = ItemWidget;

		InOperation = DragDropOp;
	}
}

bool UGSAbilityBookSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}