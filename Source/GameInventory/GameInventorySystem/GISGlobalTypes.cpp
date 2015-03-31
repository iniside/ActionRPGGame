#pragma once
#include "GameInventorySystem.h"
#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "Widgets/GISContainerBaseWidget.h"
#include "Widgets/GISItemBaseWidget.h"
#include "Widgets/GISTabBaseWidget.h"
#include "Widgets/GISSlotBaseWidget.h"
#include "GISGlobalTypes.h"

bool FGISInventoryConfiguration::IsValid()
{
	if (InventoryContainerClass && TabClass && SlotClass && ItemClass
		&& DropSlottName.IsValid())
		return true;
	return false;
}
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
	CurrentInventoryComponent->DecrementItemCount(SlotTabIndex);
}
void FGISSlotInfo::IncrementItemCount()
{
	CurrentInventoryComponent->IncrementItemCount(SlotTabIndex);
}

FGISSlotSwapInfo::FGISSlotSwapInfo(const struct FGISSlotInfo& LastSlot, class UGISItemData* LastItemDataIn,
	const struct FGISSlotInfo& TargetSlot, class UGISItemData* TargetItemDataIn)
	: LastTabIndex(LastSlot.SlotTabIndex),
	LastSlotIndex(LastSlot.SlotIndex),
	LastSlotComponent(LastSlot.CurrentInventoryComponent),
	LastSlotData(LastItemDataIn),
	TargetTabIndex(TargetSlot.SlotTabIndex),
	TargetSlotIndex(TargetSlot.SlotIndex),
	TargetSlotComponent(TargetSlot.CurrentInventoryComponent),
	TargetSlotData(TargetItemDataIn)
{}

void FGISSlotSwapInfo::LastAddItem(const FGISSlotSwapInfo& SwapInfo) const
{
	LastSlotComponent->GetInventoryWidget()->AddItem(SwapInfo);
}
void FGISSlotSwapInfo::LastRemoveItem(const FGISSlotSwapInfo& SwapInfo) const
{
	LastSlotComponent->GetInventoryWidget()->RemoveItem(SwapInfo);
}
void FGISSlotSwapInfo::TargetAddItem(const FGISSlotSwapInfo& SwapInfo) const
{
	TargetSlotComponent->GetInventoryWidget()->AddItem(SwapInfo);
}
void FGISSlotSwapInfo::TargetRemoveItem(const FGISSlotSwapInfo& SwapInfo) const
{
	TargetSlotComponent->GetInventoryWidget()->RemoveItem(SwapInfo);
}