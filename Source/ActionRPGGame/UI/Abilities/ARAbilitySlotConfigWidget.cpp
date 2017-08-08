// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilitySlotConfigWidget.h"
#include "Input/Reply.h"
#include "Input/Events.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ARGlobals.h"
#include "AFAbilityComponent.h"
#include "ARUIAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"


FReply UARAbilitySlotConfigWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//return FReply::Unhandled();
}

void UARAbilitySlotConfigWidget::NativeOnDragDetected(const FGeometry& InGeometry
	, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(UDragDropOperation::StaticClass());
	if (DragDropOp)
	{
		APlayerController* MyPC = Cast<APlayerController>(OwningComponent->GetOwner());
		UARAbilityWidget* DragIcon = CreateWidget<UARAbilityWidget>(MyPC, OwningComponent->DragVisualClass);
		DragIcon->AbilityIndex = AbilityIndex;
		DragIcon->AbilitySetIndex = AbilitySetIndex;
		DragIcon->OwningComponent = OwningComponent;

		DragDropOp->Payload = this;
		DragDropOp->DefaultDragVisual = DragIcon;

		OutOperation = DragDropOp;
	}
}
bool UARAbilitySlotConfigWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARAbilityWidget* Payload = Cast<UARAbilityWidget>(InOperation->Payload);
	Setbility(Payload->AbilityTag);

	OwningComponent->NativeEquipAbility(Payload->AbilityTag, AbilitySetIndex, AbilityIndex);
	return false;
}