// Fill out your copyright notice in the Description page of Project Settings.

#include "ARMagazineUpgradeWidget.h"
#include "Weapons/ARWeaponManagerComponent.h"
#include "Weapons/ARItemWeapon.h"



FReply UARMagazineUpgradeWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	WeaponManager->WeaponToModify->AddMagazineUpgrade(WeaponUpgrade);
	return FReply::Unhandled();
}