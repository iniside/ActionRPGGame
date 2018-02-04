// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponManagerWidget.h"

#include "../../ARPlayerController.h"
#include "../../Weapons/ARWeaponManagerComponent.h"



void UARWeaponManagerWidget::NativePreConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		WeaponManager = MyPC->WeaponManager;
	}
	Super::NativePreConstruct();
}
void UARWeaponManagerWidget::NativeConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		WeaponManager = MyPC->WeaponManager;
	}
	Super::NativeConstruct();
}