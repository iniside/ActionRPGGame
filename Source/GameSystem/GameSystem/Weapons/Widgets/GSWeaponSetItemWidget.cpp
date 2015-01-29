// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../../Items/GSItemWeaponInfo.h"
#include "Widgets/GISItemBaseWidget.h"

#include "../GSWeaponEquipmentComponent.h"

#include "GISGlobalTypes.h"
#include "GSWeaponSetItemWidget.h"

UGSWeaponSetItemWidget::UGSWeaponSetItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemSlotIndex = -1;
	ItemTabIndex = -1;
}

UTexture2D* UGSWeaponSetItemWidget::GetIcon() const
{
	if (WeaponSlot.WeaponID == -1 || WeaponSlot.WeaponTabID == -1)
		return nullptr;
	if (WeaponEquipComp->Tabs.InventoryTabs[WeaponSlot.WeaponTabID].TabSlots[WeaponSlot.WeaponID].ItemData)
	{
		return WeaponEquipComp->Tabs.InventoryTabs[WeaponSlot.WeaponTabID].TabSlots[WeaponSlot.WeaponID].ItemData->GetImage();
	}
	return nullptr;
}