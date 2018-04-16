// Fill out your copyright notice in the Description page of Project Settings.

#include "ARInventoryScreenWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

#include "UI/ARUIComponent.h"
#include "ARPlayerController.h"

void UARInventoryScreenWidget::SetWeaponName(const FString& Name)
{
	SelectedWeapon->SetText(FText::FromString(Name));
}

void UARInventoryScreenWidget::UpdateItemList(const TArray<uint8>& LocalIdxs)
{
	SelectedItemsContainer->ClearChildren();

	for (uint8 Idx : LocalIdxs)
	{
		UIFItemBase* Item = Inventory->GetItem(Idx);

		if (Item)
		{
			const FIFItemData& Slot = Inventory->GetSlot(Idx);
			UIFItemWidget* ItemWidget = CreateWidget<UIFItemWidget>(PC.Get(), UI->ItemWidgetClass);
			ItemWidget->Inventory = Inventory;
			ItemWidget->OnSlotCreated(Slot.GetNetIndex(), Slot.GetLocalIndex());
			ItemWidget->OnItemChanged(Slot.GetNetIndex(), Slot.GetLocalIndex());

			SelectedItemsContainer->AddChild(ItemWidget);
		}
	}

}