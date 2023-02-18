// Fill out your copyright notice in the Description page of Project Settings.

#include "ARInventoryScreenWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "UI/Inventory/Weapons/ARItemWeaponWidget.h"

#include "ARPlayerController.h"

void UARInventoryScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RightWeaponWidget->Index = 0;
	LeftWeaponWidget->Index = 1;
	SideWeaponWidget->Index = 2;
	BottomBackWeaponWidget->Index = 3;

	ModifySelectedWeapon->OnClicked.AddDynamic(this, &UARInventoryScreenWidget::OnModifyWeaponClicked);
}

void UARInventoryScreenWidget::SetWeaponName(const FString& Name)
{
	SelectedWeapon->SetText(FText::FromString(Name));
}


void UARInventoryScreenWidget::OnModifyWeaponClicked()
{
	Inventory->ModifyWeapon();
}