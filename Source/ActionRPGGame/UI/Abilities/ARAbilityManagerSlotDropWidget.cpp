// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityManagerSlotDropWidget.h"

#include "ARAbilityManagerSlotDragWidget.h"
#include "../../Abilities/ARAbilityManagerComponent.h"

bool UARAbilityManagerSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARAbilityManagerSlotDragWidget* Payload = Cast<UARAbilityManagerSlotDragWidget>(InOperation->Payload);

	AbilityManager->NativeEquipAbility(Payload->GetAbilityTag(), AbilityGroup, AbilitySlot);
	return false;
}