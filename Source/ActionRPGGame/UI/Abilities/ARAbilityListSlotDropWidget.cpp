// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityListSlotDropWidget.h"

#include "ARAbilityListSlotDragWidget.h"
#include "Abilities/ARAbilityManagerComponent.h"

bool UARAbilityListSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARAbilityListSlotDragWidget* Payload = Cast<UARAbilityListSlotDragWidget>(InOperation->Payload);

	AbilityManager->NativeEquipAbility(Payload->GetAbilityTag(), AbilityGroup, AbilitySlot);
	return true;
}