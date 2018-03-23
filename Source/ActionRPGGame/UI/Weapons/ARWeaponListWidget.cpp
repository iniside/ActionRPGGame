// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponListWidget.h"

#include "ARPlayerController.h"
#include "Weapons/ARWeaponManagerComponent.h"



void UARWeaponListWidget::NativePreConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		ARPC = MyPC;
		WeaponManager = MyPC->WeaponManager;
	}
	Super::NativePreConstruct();
}
void UARWeaponListWidget::NativeConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		ARPC = MyPC;
		WeaponManager = MyPC->WeaponManager;
	}
	Super::NativeConstruct();
}
void UARWeaponListWidget::AddItem(TSubclassOf<class UARWeaponListSlotDragWidget> DragWidgetClass, int32 WeaponIdx)
{
	UARWeaponListSlotDragWidget* Item = CreateWidget<UARWeaponListSlotDragWidget>(ARPC.Get(), DragWidgetClass);
	Item->WeaponManager = WeaponManager;
	Item->WeaponIdx = WeaponIdx;
	Items.Add(Item);
	Item->OnItemAdded();
	ItemContainer->AddChild(Item);
}

