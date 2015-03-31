#pragma once
#include "GameInventorySystem.h"
#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "GISGlobalTypes.h"

bool FGISSlotInfo::IsValid()
{
	if (!CurrentInventoryComponent.IsValid())
		return false;
	if (!CurrentInventoryComponent->Tabs.InventoryTabs.IsValidIndex(SlotTabIndex))
		return false;
	if (!CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].TabSlots.IsValidIndex(SlotIndex))
		return false;

	return true;
}

class UGISItemData* FGISSlotInfo::GetItemData()
{
	UGISItemData* returnData = nullptr;

	returnData = CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].TabSlots[SlotIndex].ItemData;

	return returnData;
}

void FGISSlotInfo::SetItemData(class UGISItemData* DataIn)
{
	CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].TabSlots[SlotIndex].ItemData = DataIn;
}

void FGISSlotInfo::DecrementItemCount()
{
	CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].ItemCount--;
}
void FGISSlotInfo::IncrementItemCount()
{
	CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].ItemCount++;
}