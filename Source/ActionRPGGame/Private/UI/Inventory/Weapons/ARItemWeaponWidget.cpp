// Fill out your copyright notice in the Description page of Project Settings.

#include "ARItemWeaponWidget.h"

#include "IFInventoryComponent.h"

#include "ARPlayerController.h"
#include "ARCharacter.h"

#include "Weapons/ARItemWeapon.h"
#include "Weapons/ARWeaponInventoryComponent.h"

#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "UI/Inventory/ARInventoryScreenWidget.h"
#include "UI/Inventory/ARItemTooltipView.h"
#include "UI/Inventory/Weapons/ARWeaponContainerWidget.h"
#include "UI/Inventory/Weapons/ARListItemWeaponWidget.h"

void UARItemWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(GetOwningPlayer())
	{
		if (AARPlayerController* PC = Cast<AARPlayerController>(GetOwningPlayer()))
		{
			Inventory = PC->MainInventory;
			if (AARCharacter* Character = Cast<AARCharacter>(PC->GetPawn()))
			{
				Character->WeaponInventory->GetOnItemAdded().AddUObject(this, &UARItemWeaponWidget::OnWeaponAdded2);
				Character->WeaponInventory->GetOnItemUpdated().AddUObject(this, &UARItemWeaponWidget::OnWeaponUpdated2);
				Character->WeaponInventory->GetOnItemRemoved().AddUObject(this, &UARItemWeaponWidget::OnWeaponRemoved2);
			}
		}
		if (AARHUD* HUD = Cast<AARHUD>(GetOwningPlayer()->GetHUD()))
		{
			InventoryComponent = HUD->GetUIInventory();
			if (AARCharacter* Character = Cast<AARCharacter>(GetOwningPlayer()->GetPawn()))
			{
				WeaponInventory = Character->WeaponInventory;
			}
		}
	}
}


FReply UARItemWeaponWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UObject* Out = GetOuter();
		UARInventoryScreenWidget* MainView = Cast<UARInventoryScreenWidget>(Out);
		InventoryComponent->ShowWeaponsForSlot(this);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		InventoryComponent->RemoveWeaponFromSlot(LocalIndex);
	}
	

	return FReply::Handled();
}
void UARItemWeaponWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!WeaponItem.IsValid())
		return;

	if(!WeaponTooltip)
		WeaponTooltip = CreateWidget<UARItemTooltipView>(GetOwningPlayer(), InventoryComponent->WeaponItemTooltipViewClass);

	WeaponTooltip->OnTooltipCreated(WeaponItem->GetTooltipData());

	SetToolTip(WeaponTooltip);

	/*FMargin margin = GetFullScreenOffset();
	FVector2D Pos(margin.Left, margin.Top);
	WeaponTooltip->AddToViewport(9999);
	
	WeaponTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
	WeaponTooltip->SetPositionInViewport(Pos);*/
}
void UARItemWeaponWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!WeaponTooltip)
		return;

	SetToolTip(nullptr);
	//WeaponTooltip->SetVisibility(ESlateVisibility::Collapsed);
}
FReply UARItemWeaponWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Handled = FReply::Unhandled();

	InventoryComponent->UnequipWeaponFromSlot(NetIndex, LocalIndex);
	Handled = FReply::Handled();

	return Handled;
}


void UARItemWeaponWidget::OnWeaponAdded2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem)
{
	if (InLocalIndex == Index)
	{
		OnSlotCreated(InNetIndex, InLocalIndex, InItem);
		WeaponItem = Cast<UARItemWeapon>(InItem);
	}
}
void UARItemWeaponWidget::OnWeaponUpdated2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem)
{
	if (InLocalIndex == Index)
	{
		OnItemChanged(InNetIndex, InLocalIndex, InItem);
	}
}
void UARItemWeaponWidget::OnWeaponRemoved2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem)
{
	if (InLocalIndex == Index)
	{
		OnItemRemoved(InNetIndex, InLocalIndex, InItem);
	}
	WeaponItem.Reset();
}