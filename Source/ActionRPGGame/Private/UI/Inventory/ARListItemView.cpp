// Fill out your copyright notice in the Description page of Project Settings.

#include "ARListItemView.h"

#include "ARItemBase.h"
#include "UI/ARHUD.h"
#include "UI/Inventory/ARUIInventoryComponent.h"
#include "ARPlayerController.h"


void UARListItemView::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer())
	{
		if (AARHUD* HUD = Cast<AARHUD>(GetOwningPlayer()->GetHUD()))
		{
			InventoryComponent = HUD->GetUIInventory();
		}
	}
}

void UARListItemView::OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	Super::OnSlotCreated(InNetIndex, InLocalIndex, Item);
	InvItem = Cast<UARItemBase>(Item);
}
void UARListItemView::OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	Super::OnItemChanged(InNetIndex, InLocalIndex, Item);
}
void UARListItemView::OnItemRemoved(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	Super::OnItemRemoved(InNetIndex, InLocalIndex, Item);
	InvItem.Reset();
}

void UARListItemView::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InvItem.IsValid())
		return;

	if (!ItemTooltip)
		ItemTooltip = CreateWidget<UARItemTooltipView>(GetOwningPlayer(), InventoryComponent->WeaponItemTooltipViewClass);

	ItemTooltip->OnTooltipCreated(InvItem->GetTooltipData());

	SetToolTip(ItemTooltip);
}
void UARListItemView::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!ItemTooltip)
		return;

	SetToolTip(nullptr);
}