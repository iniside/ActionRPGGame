// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponUpgradeListWidget.h"

#include "ARPlayerController.h"
#include "Weapons/ARMagazineUpgradeItem.h"
#include "UI/Weapons/ARMagazineUpgradeWidget.h"
#include "Components/WrapBox.h"
#include "Components/Image.h"
#include "Weapons/ARWeaponManagerComponent.h"



void UARWeaponUpgradeListWidget::OnShow()
{
	UpgradeList.Empty();
	ItemContainer->ClearChildren();

	for (const TSubclassOf<UARMagazineUpgradeItem>& Upgrade : WeaponManager->MagazineUpgradesClasses)
	{
		UARMagazineUpgradeItem* CDO = Upgrade->GetDefaultObject<UARMagazineUpgradeItem>();

		UARMagazineUpgradeWidget* Item = CreateWidget<UARMagazineUpgradeWidget>(Cast<APlayerController>(WeaponManager->GetOwner()), WeaponManager->MagazineUpgradeClass);
		Item->WeaponManager = WeaponManager;
		Item->WeaponUpgrade = Upgrade;
		Item->Icon->SetBrushFromTexture(CDO->Icon);
		UpgradeList.Add(Item);
		ItemContainer->AddChild(Item);
	}
}