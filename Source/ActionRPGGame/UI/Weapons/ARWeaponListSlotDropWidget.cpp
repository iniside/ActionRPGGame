// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponListSlotDropWidget.h"

#include "AFAbilityComponent.h"

#include "Weapons/ARWeaponManagerComponent.h"
#include "Weapons/ARWeaponAbilityBase.h"
#include "ARWeaponListSlotDragWidget.h"

bool UARWeaponListSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARWeaponListSlotDragWidget* Payload = Cast<UARWeaponListSlotDragWidget>(InOperation->Payload);
	FSlateBrush Brush = Payload->IconImage->Brush;

	IconImage->SetBrush(Brush);
	WeaponManager->AddWeaponToManager(WeaponSlot, EAMSlot::Slot001, Payload->GetWeapon());
	return true;
}