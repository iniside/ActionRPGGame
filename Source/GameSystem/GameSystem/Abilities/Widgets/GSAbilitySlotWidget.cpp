// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GSAbilityBookItem.h"
#include "../GSAbilitiesComponent.h"
#include "GSAbilitySlotWidget.h"

UGSAbilitySlotWidget::UGSAbilitySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FReply UGSAbilitySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	return Reply;
}

void UGSAbilitySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{

}

bool UGSAbilitySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UGSAbilityBookItem* bookItem = Cast<UGSAbilityBookItem>(InOperation->Payload))
	{
		OwnerComp->AddAbilityToSlotFromBook(SlotInfo.SetIndex, SlotInfo.SlotIndex, bookItem->SlotInfo.TabIndex, bookItem->SlotInfo.SlotIndex);
	}
	return false;
}