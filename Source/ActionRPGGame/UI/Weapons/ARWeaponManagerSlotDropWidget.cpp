// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerSlotDropWidget.h"

#include "AFAbilityComponent.h"

#include "../../Weapons/ARWeaponManagerComponent.h"
#include "../../Weapons/ARWeaponAbilityBase.h"
#include "ARWeaponManagerSlotDragWidget.h"

bool UARWeaponManagerSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARWeaponManagerSlotDragWidget* Payload = Cast<UARWeaponManagerSlotDragWidget>(InOperation->Payload);

	WeaponManager->AddWeaponToManager(WeaponSlot, EAMSlot::Slot001, Payload->GetWeapon());
	return false;
}