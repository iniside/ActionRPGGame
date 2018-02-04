// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerSlotDragWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ARAbilityDragVisual.h"
#include "../../Weapons/ARWeaponManagerComponent.h"

FReply UARWeaponManagerSlotDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARWeaponManagerSlotDragWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(WeaponManager->GetOwner());
		//UARAbilityDragVisual* DragIcon = CreateWidget<UARAbilityDragVisual>(MyPC, AbilityManager->GetDragVisualClass());
		//DragIcon->AbilityManager = AbilityManager;

		DragDropOp->Payload = this;
		//DragDropOp->DefaultDragVisual = DragIcon;

		OutOperation = DragDropOp;
	}
}
