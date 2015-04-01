#pragma once
#include "GameInventorySystem.h"
#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "Widgets/GISContainerBaseWidget.h"
#include "Widgets/GISItemBaseWidget.h"
#include "Widgets/GISTabBaseWidget.h"
#include "Widgets/GISSlotBaseWidget.h"
#include "Widgets/GISLootContainerBaseWidget.h"
#include "Widgets/GISLootSlotBaseWidget.h"
#include "GISGlobalTypes.h"

bool FGISInventoryConfiguration::IsValid()
{
	if (InventoryContainerClass && TabClass && SlotClass && ItemClass
		&& DropSlottName.IsValid())
		return true;
	return false;
}

bool FGISLootConfiguration::IsValid()
{
	if (LootWidgetClass && LootSlotClass && LootItemClass
		&& LootItemSlotName.IsValid())
		return true;

	return false;
}

bool FGISSlotInfo::IsValid() const
{
	if (!CurrentInventoryComponent.IsValid())
		return false;
	if (!CurrentInventoryComponent->IsInventoryValid(SlotTabIndex, SlotIndex))
		return false;

	return true;
}

class UGISItemData* FGISSlotInfo::GetItemData() const
{
	UGISItemData* returnData = nullptr;

	returnData = CurrentInventoryComponent->GetItemDataInSlot(SlotTabIndex,SlotIndex);

	return returnData;
}

void FGISSlotInfo::SetItemData(class UGISItemData* DataIn) const
{
	CurrentInventoryComponent->SetItemDataInSlot(SlotTabIndex,SlotIndex, DataIn);
}

void FGISSlotInfo::DecrementItemCount() const
{
	CurrentInventoryComponent->DecrementItemCount(SlotTabIndex);
}
void FGISSlotInfo::IncrementItemCount() const
{
	CurrentInventoryComponent->IncrementItemCount(SlotTabIndex);
}
void FGISSlotInfo::EnsureReplication() const
{
	CurrentInventoryComponent->EnsureInventoryReplication();
}
FGameplayTagContainer& FGISSlotInfo::GetTags() const
{
	return CurrentInventoryComponent->GetTabTags(SlotTabIndex);
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