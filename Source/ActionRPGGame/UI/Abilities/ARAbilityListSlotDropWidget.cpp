// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityListSlotDropWidget.h"

#include "ARAbilityListSlotDragWidget.h"
#include "Abilities/ARAbilityManagerComponent.h"
#include "UI/ARUIComponent.h"

bool UARAbilityListSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARAbilityListSlotDragWidget* Payload = Cast<UARAbilityListSlotDragWidget>(InOperation->Payload);
	FSlateBrush brush = Payload->IconImage->Brush;
	
	AbilityManager->NativeEquipAbility(Payload->GetAbilityTag(), AbilityGroup, AbilitySlot);

	IconImage->SetBrush(brush);
	if (AbilityGroup == EAMGroup::Group001)
	{
		switch (AbilitySlot)
		{
		case EAMSlot::Slot001:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup001Slot001->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		case EAMSlot::Slot002:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup001Slot002->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		case EAMSlot::Slot003:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup001Slot003->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		default:
			break;
		}
	}
	else if (AbilityGroup == EAMGroup::Group002)
	{
		switch (AbilitySlot)
		{
		case EAMSlot::Slot001:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup002Slot001->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		case EAMSlot::Slot002:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup002Slot002->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		case EAMSlot::Slot003:
			UIComponent->HUDWidget->PlayerInfo->AbilityGroup002Slot003->AbilityIcon->SetBrush(brush); //DAT BAD
			break;
		default:
			break;
		}
	}
	
	return true;
}