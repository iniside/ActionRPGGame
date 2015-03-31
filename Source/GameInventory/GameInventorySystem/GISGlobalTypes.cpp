#pragma once
#include "GameInventorySystem.h"
#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "GISGlobalTypes.h"

bool FGISSlotInfo::IsValid()
{
	if (!CurrentInventoryComponent.IsValid())
		return false;
	if (!CurrentInventoryComponent->IsInventoryValid(SlotTabIndex, SlotIndex))
		return false;

	return true;
}

class UGISItemData* FGISSlotInfo::GetItemData()
{
	UGISItemData* returnData = nullptr;

	returnData = CurrentInventoryComponent->GetItemDataInSlot(SlotTabIndex,SlotIndex);

	return returnData;
}

void FGISSlotInfo::SetItemData(class UGISItemData* DataIn)
{
	CurrentInventoryComponent->SetItemDataInSlot(SlotTabIndex,SlotIndex, DataIn);
}

void FGISSlotInfo::DecrementItemCount()
{
	CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].ItemCount--;
}
void FGISSlotInfo::IncrementItemCount()
{
	CurrentInventoryComponent->Tabs.InventoryTabs[SlotTabIndex].ItemCount++;
}