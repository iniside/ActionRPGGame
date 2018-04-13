// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "IFInventoryComponent.h"

void UIFItemWidget::OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex)
{
	NetIndex = InNetIndex;
	LocalIndex = InLocalIndex;
	//const UIFItemBase* Item = Inventory->GetItem(InLocalIndex);
	//BP_OnItemCreated(const_cast<UIFItemBase*>(Item));
}

void UIFItemWidget::OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex)
{
	const UIFItemBase* Item = Inventory->GetItem(InLocalIndex);
	BP_OnItemChanged(const_cast<UIFItemBase*>(Item));
}

FReply UIFItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}
void UIFItemWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* PC = GetOwningPlayer();

		DragDropOp->Payload = this;
		DragDropOp->DefaultDragVisual = DragVisual;

		OutOperation = DragDropOp;
	}
}
bool UIFItemWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UIFItemWidget* Payload = Cast<UIFItemWidget>(InOperation->Payload);

	Inventory->MoveItemInInventory(LocalIndex, Payload->LocalIndex);
	return true;
}