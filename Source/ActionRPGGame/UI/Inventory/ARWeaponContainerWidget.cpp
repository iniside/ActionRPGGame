// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponContainerWidget.h"

#include "IFItemWidget.h"

#include "ARPlayerController.h"
#include "ARCharacter.h"

#include "UI/ARUIComponent.h"
#include "UI/Weapons/ARItemWeaponWidget.h"


void UARWeaponContainerWidget::InitializeWeaponItems(class UARUIComponent* UIComponent)
{
	for (UIFItemWidget* Widget : InventoryWidgets)
	{
		if (UARItemWeaponWidget* Weapon = Cast<UARItemWeaponWidget>(Widget))
		{
			AARPlayerController* PC = Cast<AARPlayerController>(UIComponent->GetOwner());
			AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn());

			Weapon->WeaponInventory = Character->WeaponInventory;
			Weapon->UI = UIComponent;
			Weapon->InventoryWidget = UIComponent->InventoryScreenWidget;
		}
	}
}