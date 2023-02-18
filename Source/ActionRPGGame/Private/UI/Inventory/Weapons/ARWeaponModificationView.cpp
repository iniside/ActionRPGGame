// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponModificationView.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARMagazineUpgradeItem.h"

void UARWeaponModificationView::NativeConstruct()
{
	Super::NativeConstruct();
	CloseUpgradeView->OnClicked.AddDynamic(this, &UARWeaponModificationView::CloseModificationView);
}
void UARWeaponModificationView::StartModifyWeapon(class UARItemWeapon* WeaponItem)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
void UARWeaponModificationView::StopModifyWeapon()
{

}

void UARWeaponModificationView::CloseModificationView()
{
	StopModifyWeapon();
	SetVisibility(ESlateVisibility::Collapsed);
}