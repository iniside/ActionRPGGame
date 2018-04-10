// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponListWidget.h"

#include "ARPlayerController.h"
#include "ARWeaponUpgradeListWidget.h"
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
	Magazine->OnClicked.AddDynamic(this, &UARWeaponListWidget::OnMagazineClicked);
	Super::NativeConstruct();
}
void UARWeaponListWidget::AddItem(TSubclassOf<class UARWeaponListSlotDragWidget> DragWidgetClass, int32 WeaponIdx)
{
	UARWeaponListSlotDragWidget* Item = CreateWidget<UARWeaponListSlotDragWidget>(ARPC.Get(), DragWidgetClass);
	Item->WeaponManager = WeaponManager;
	Item->WeaponIdx = WeaponIdx;
	Item->WeaponList = this;
	Items.Add(Item);
	Item->OnItemAdded();
	ItemContainer->AddChild(Item);
}

void UARWeaponListWidget::OnMagazineClicked()
{
	if (WeaponManager->WeaponToModify.IsValid())
	{
		WeaponManager->MagazineUpgradeListWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WeaponManager->MagazineUpgradeListWidget->OnShow();
	}
}