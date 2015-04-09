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

FEventReply UGSEquipmentSlotWidget::OnMouseButtonDown_Implementation(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	return Reply;
}

void UGSEquipmentSlotWidget::OnDragDetected_Implementation(FGeometry MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& Operation)
{

}

bool UGSEquipmentSlotWidget::OnDrop_Implementation(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation)
{
	if (UGISItemBaseWidget* Item = Cast<UGISItemBaseWidget>(Operation->Payload))
	{
		return true;
	}
	if (UGSEquipmentItemWidget* Item = Cast<UGSEquipmentItemWidget>(Operation->Payload))
	{
		return true;
	}
	return false;
}