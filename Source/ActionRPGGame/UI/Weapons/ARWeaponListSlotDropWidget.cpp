// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponListSlotDropWidget.h"

#include "Components/TextBlock.h"

#include "AFAbilityComponent.h"

#include "ARCharacter.h"
#include "ARPlayerController.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARWeaponManagerComponent.h"
#include "Weapons/ARWeaponAbilityBase.h"

#include "ARWeaponListWidget.h"

#include "ARWeaponListSlotDragWidget.h"

bool UARWeaponListSlotDropWidget::NativeOnDrop(const FGeometry& InGeometry
	, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UARWeaponListSlotDragWidget* Payload = Cast<UARWeaponListSlotDragWidget>(InOperation->Payload);
	FSlateBrush Brush = Payload->IconImage->Brush;

	IconImage->SetBrush(Brush);
	WeaponList = Payload->WeaponList;

	WeaponManager->AddWeaponToManager(WeaponSlot, EAMSlot::Slot001, Payload->GetWeapon());
	return true;
}

FReply UARWeaponListSlotDropWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	AARPlayerController* PC = Cast<AARPlayerController>(WeaponManager->GetOwner());

	AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());
	UGAAbilityBase* WeaponAbility = WeaponManager->GetAbility(WeaponSlot, EAMSlot::Slot001);
	
	WeaponList->SelectedWeaponName->SetText(FText::FromString(Character->WeaponRightItem->GetName()));
	WeaponManager->WeaponToModify = Character->WeaponRightItem;

	return FReply::Unhandled();
}