// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../../Items/GSItemWeaponInfo.h"
#include "../GSWeaponEquipmentComponent.h"

#include "GSWeaponSetWidget.h"
#include "GSWeaponSetSlotWidget.h"
#include "GSWeaponSetItemWidget.h"

#include "GSWeaponSetContainerWidget.h"

UGSWeaponSetContainerWidget::UGSWeaponSetContainerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
void UGSWeaponSetContainerWidget::InitializeContainer()
{
	WeaponEquipComp->OnWeaponSetsInitialized.BindUObject(this, &UGSWeaponSetContainerWidget::InitializeInventory);
	WeaponEquipComp->OnWeaponSetUpdated.BindUObject(this, &UGSWeaponSetContainerWidget::UpdateWeaponSet);
}

void UGSWeaponSetContainerWidget::InitializeInventory()
{
	if (!WeaponEquipComp)
		return;
	if (!SetWidgetClass || !ItemWidgetClass || !SlotWidgetClass)
		return;

	UWorld* world = WeaponEquipComp->GetWorld();

	for (const FGSWeaponSet& set : WeaponEquipComp->WeaponSets)
	{
		UGSWeaponSetWidget* setWidget = CreateWidget<UGSWeaponSetWidget>(world, SetWidgetClass);
		if (setWidget)
		{
			for (const FGSWeaponSetSlot& slot : set.Weapons)
			{
				UGSWeaponSetSlotWidget* weaponSlot = CreateWidget<UGSWeaponSetSlotWidget>(world, SlotWidgetClass);
				if (weaponSlot)
				{
					UGSWeaponSetItemWidget* weapItem = CreateWidget<UGSWeaponSetItemWidget>(world, ItemWidgetClass);
					weaponSlot->ItemWidget = weapItem;
					weaponSlot->WeaponEquipComp = WeaponEquipComp;
					weaponSlot->DropSlottName = DropSlottName;
					weapItem->WeaponEquipComp = WeaponEquipComp;
					weaponSlot->WeaponSlot = slot;
					UWidget* dropWidget = weaponSlot->GetWidgetFromName(DropSlottName);
					UOverlay* overlay = Cast<UOverlay>(dropWidget);

					overlay->AddChild(weapItem);
					weapItem->SetVisibility(ESlateVisibility::Collapsed);

					setWidget->WeaponSlots.Add(weaponSlot);
				}
			}
			WeaponSets.Add(setWidget);
		}
	}
}
void UGSWeaponSetContainerWidget::UpdateWeaponSet()
{
	//reset everything.
	for (UGSWeaponSetWidget* set : WeaponSets)
	{
		for (UGSWeaponSetSlotWidget* slot : set->WeaponSlots)
		{
			slot->ItemWidget->SetVisibility(ESlateVisibility::Collapsed);
			slot->WeaponSlot.WeaponID = -1;
			slot->WeaponSlot.WeaponTabID = -1;
			slot->ItemWidget->WeaponSlot.WeaponID = -1;
			slot->ItemWidget->WeaponSlot.WeaponTabID = -1;
		}
	}
	//reconstruct everything;
	for (const FGSWeaponSet& set : WeaponEquipComp->WeaponSets)
	{
		for (const FGSWeaponSetSlot& slot : set.Weapons)
		{
			WeaponSets[slot.SetIndex]->WeaponSlots[slot.SlotIndex]->WeaponSlot = slot;
			if (slot.WeaponID >= 0 && slot.WeaponTabID >= 0)
			{
				WeaponSets[slot.SetIndex]->WeaponSlots[slot.SlotIndex]->ItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				WeaponSets[slot.SetIndex]->WeaponSlots[slot.SlotIndex]->ItemWidget->WeaponSlot = slot;
			}
		}
	}
	//That's probabaly not as efficient as updating slots individually but:
	//1. It's going to be cheaper on replication.
	//2. There are like 8 tabs tops, which means 16 slots. REally, if your CPU can't handle it, go play saper.
}