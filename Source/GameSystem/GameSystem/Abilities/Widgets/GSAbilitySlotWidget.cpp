// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GSAbilityBookItem.h"
#include "../GSAbilitiesComponent.h"
#include "GSAbilitySlotWidget.h"

UGSAbilitySlotWidget::UGSAbilitySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FEventReply UGSAbilitySlotWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	return Reply;
}

void UGSAbilitySlotWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{

}

bool UGSAbilitySlotWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	if (UGSAbilityBookItem* bookItem = Cast<UGSAbilityBookItem>(Operation->Payload))
	{
		OwnerComp->AddAbilityToSlotFromBook(SlotInfo.SetIndex, SlotInfo.SlotIndex, bookItem->SlotInfo.TabIndex, bookItem->SlotInfo.SlotIndex);
	}
	return false;
}