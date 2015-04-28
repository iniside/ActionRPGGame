// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GISInventoryBaseComponent.h"
#include "../GSEqiupmentComponent2.h"
#include "Widgets/GISItemBaseWidget.h"

#include "GSEquipmentItemWidget.h"

#include "GSEquipmentSlotWidget.h"

UGSEquipmentSlotWidget::UGSEquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FReply UGSEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();
	return Reply;
}

void UGSEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{

}

bool UGSEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UGISItemBaseWidget* Item = Cast<UGISItemBaseWidget>(InOperation->Payload))
	{
		return true;
	}
	if (UGSEquipmentItemWidget* Item = Cast<UGSEquipmentItemWidget>(InOperation->Payload))
	{
		return true;
	}
	return false;
}