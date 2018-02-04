// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityManagerSlotDragWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ARAbilityDragVisual.h"
#include "../../Abilities/ARAbilityManagerComponent.h"

FReply UARAbilityManagerSlotDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARAbilityManagerSlotDragWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(AbilityManager->GetOwner());
		UARAbilityDragVisual* DragIcon = CreateWidget<UARAbilityDragVisual>(MyPC, AbilityManager->GetDragVisualClass());
		DragIcon->AbilityManager = AbilityManager;

		DragDropOp->Payload = this;
		DragDropOp->DefaultDragVisual = DragIcon;

		OutOperation = DragDropOp;
	}
}
