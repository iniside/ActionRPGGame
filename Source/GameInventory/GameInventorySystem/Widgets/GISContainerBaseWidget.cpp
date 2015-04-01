// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "GISSlotBaseWidget.h"
#include "GISItemBaseWidget.h"
#include "GISTabBaseWidget.h"

#include "../GISGlobalTypes.h"
#include "../GISInventoryBaseComponent.h"

#include "GISContainerBaseWidget.h"

UGISContainerBaseWidget::UGISContainerBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGISContainerBaseWidget::InitializeContainer(const FGISInventoryConfiguration& ConfigIn,
class UGISInventoryBaseComponent* InventoryComponentIn, APlayerController* PCOwnerIn)
{
	Config = ConfigIn;
	TabClass = ConfigIn.TabClass;
	SlotClass = ConfigIn.SlotClass;
	ItemClass = ConfigIn.ItemClass;
	DropSlottName = ConfigIn.DropSlottName;
	InventoryComponent = InventoryComponentIn;
	PCOwner = PCOwnerIn;
	if (InventoryComponent)
	{
		//bind functions to delegates:
		InventoryComponent->OnInventoryLoaded.AddDynamic(this, &UGISContainerBaseWidget::InitializeInventory);
		InventoryComponent->OnItemAdded.AddDynamic(this, &UGISContainerBaseWidget::Widget_OnItemAdded);
		InventoryComponent->OnItemSlotSwapped.AddDynamic(this, &UGISContainerBaseWidget::Widget_OnItemSlotSwapped);
		InventoryComponent->OnTabVisibilityChanged.BindUObject(this, &UGISContainerBaseWidget::Widget_OnTabVisibilityChanged);
		InventoryComponent->OnTabChanged.BindUObject(this, &UGISContainerBaseWidget::Widget_OnTabChanged);
		InitializeInventory();
	}
}
void UGISContainerBaseWidget::InitializeInventory()
{
	if (InventoryComponent && InventoryTabs.Num() == 0 && Config.IsValid())
	{
		int32 IndexCounter = 0;
		TArray<FGISTabInfo>& Tabs = InventoryComponent->GetInventoryTabs();
		for (const FGISTabInfo& Tab : Tabs)
		{
			UGISTabBaseWidget* tabWidget = CreateWidget<UGISTabBaseWidget>(PCOwner, Config.TabClass);
			if (tabWidget)
			{
				tabWidget->TabInfo = Tab;
				//tabWidget->SetVisibility(ESlateVisibility::Hidden);
				for (const FGISSlotInfo& Slot : Tab.TabSlots)
				{
					UGISSlotBaseWidget* slotWidget = CreateWidget<UGISSlotBaseWidget>(PCOwner, Config.SlotClass);
					if (slotWidget)
					{
						slotWidget->SlotInfo = Slot;
						slotWidget->TabInfo = Tab;
						slotWidget->DropSlottName = Config.DropSlottName;

						UGISItemBaseWidget* ItemWidget = CreateWidget<UGISItemBaseWidget>(PCOwner, Config.ItemClass);
						ItemWidget->ItemData = nullptr;
						ItemWidget->InventoryComponent = InventoryComponent;
						ItemWidget->InitializeItem();
						ItemWidget->SetVisibility(ESlateVisibility::Collapsed);
						UWidget* superWidget = slotWidget->GetWidgetFromName(Config.DropSlottName);
						UOverlay* overlay = Cast<UOverlay>(superWidget);
						overlay->AddChild(ItemWidget);

						slotWidget->ItemInSlot = ItemWidget;

						tabWidget->InventorySlots.Add(slotWidget);
					}
				}

				InventoryTabs.Add(tabWidget);
			}
		}
	}
}
void UGISContainerBaseWidget::Widget_OnItemAdded(const FGISSlotUpdateData& SlotUpdateInfo)
{
	/*
		if Item count between widget and actual inventory is different
		it means, that to inventory on server multiple items have been added
		and we need to reconstruct all slots.
		*/
	if (InventoryComponent->GetItemCount(SlotUpdateInfo.TabIndex)
		!= GetItemCount(SlotUpdateInfo.TabIndex))
	{
		Widget_OnTabChanged(SlotUpdateInfo.TabIndex);
	}

	//	UGISItemData* itemData = SlotUpdateInfo.SlotComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].TabSlots[SlotUpdateInfo.SlotIndex].ItemData;
	if (SlotUpdateInfo.SlotData && ItemClass)
	{

		FGISSlotInfo SlotInfo(SlotUpdateInfo);
		IncrementItemCount(SlotUpdateInfo.TabIndex);
		SetSlotInfo(SlotUpdateInfo.GetSlotIndex(), SlotInfo);

		UGISItemBaseWidget* ItemWidget = GetItemWidget(SlotUpdateInfo.GetSlotIndex());
		UpdateItemWidget(ItemWidget, SlotInfo);
	}
}
void UGISContainerBaseWidget::Widget_OnItemSlotSwapped(const FGISSlotSwapInfo& SlotSwapInfo)
{
	if (SlotSwapInfo.LastSlotComponent == SlotSwapInfo.TargetSlotComponent)
	{
		//	if (!SlotSwapInfo.LastSlotData)
		//	{
		RemoveItem(SlotSwapInfo);
		AddItem(SlotSwapInfo);
		//	}
	}
	//so we targeted different component with out drop action.
	//we need to handle it! but how...
	if (SlotSwapInfo.LastSlotComponent != SlotSwapInfo.TargetSlotComponent)
	{
		if (SlotSwapInfo.LastSlotComponent.IsValid() && SlotSwapInfo.TargetSlotComponent.IsValid()
			&& SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
		{
			//actually, probabaly need separate functions, as there might be more
			//complex scases like actuall swapping items, instead of puting it
			//in empty slot in another component.
			SlotSwapInfo.LastRemoveItem(SlotSwapInfo);
			SlotSwapInfo.TargetAddItem(SlotSwapInfo);
		}
		else if (!SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
		{
			SlotSwapInfo.TargetRemoveItem(SlotSwapInfo);
			SlotSwapInfo.TargetAddItem(SlotSwapInfo);
		}
	}
}

void UGISContainerBaseWidget::AddItem(const FGISSlotSwapInfo& SlotSwapInfo)
{
	//this function:
	//1. Clearly needs to be bigger!
	//2. Have even more if's!

	UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
	if (!SlotSwapInfo.LastSlotData)
	{
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.SetFromTarget(SlotSwapInfo);
		SetSlotInfo(SlotSwapInfo.GetTargetSlotIndex(), TargetSlotInfo);

		UGISItemBaseWidget* ItemWidget = GetItemWidget(SlotSwapInfo.GetTargetSlotIndex());
		
		UpdateItemWidget(ItemWidget, TargetSlotInfo);
	}
	else if (SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
	{
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.SetFromTarget(SlotSwapInfo);
		SetSlotInfo(SlotSwapInfo.GetTargetSlotIndex(), TargetSlotInfo);
		SetSlotData(SlotSwapInfo.GetTargetSlotIndex(), SlotSwapInfo.TargetSlotData);

		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.SetFromLast(SlotSwapInfo);
		SetSlotInfo(SlotSwapInfo.GetLastSlotIndex(), LastSlotInfo);
		SetSlotData(SlotSwapInfo.GetLastSlotIndex(), SlotSwapInfo.LastSlotData);

		UGISItemBaseWidget* TargetItemWidget = GetItemWidget(SlotSwapInfo.GetTargetSlotIndex());
		UpdateItemWidget(TargetItemWidget, TargetSlotInfo);
		
		UGISItemBaseWidget* LastItemWidget = GetItemWidget(SlotSwapInfo.GetLastSlotIndex());
		UpdateItemWidget(LastItemWidget, LastSlotInfo);

	}
	else
	{
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.SetFromTarget(SlotSwapInfo);
		SetSlotInfo(SlotSwapInfo.GetTargetSlotIndex(), TargetSlotInfo);
		UGISItemBaseWidget* TargetItemWidget = GetItemWidget(SlotSwapInfo.GetTargetSlotIndex());
		UpdateItemWidget(TargetItemWidget, TargetSlotInfo);
		
	}
}
void UGISContainerBaseWidget::RemoveItem(const FGISSlotSwapInfo& SlotSwapInfo)
{
	if (!SlotSwapInfo.LastSlotData && SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
	{
		//InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->bIsEnabled = false;
		//this it bit fiddly since the widget which will contain our widget must be last
		//and out item widget must be last child within this widget
		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.SetFromLast(SlotSwapInfo);

		SetSlotInfo(SlotSwapInfo.GetLastSlotIndex(), LastSlotInfo);

		UGISItemBaseWidget* itemTemp = GetItemWidget(SlotSwapInfo.GetLastSlotIndex());
		itemTemp->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
	{
		UGISItemBaseWidget* LastItem = GetItemWidget(SlotSwapInfo.GetLastSlotIndex());
		LastItem->SetVisibility(ESlateVisibility::Collapsed);

		UGISItemBaseWidget* TargetITem = GetItemWidget(SlotSwapInfo.GetTargetSlotIndex());
		TargetITem->SetVisibility(ESlateVisibility::Collapsed);

		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.SetFromTarget(SlotSwapInfo);
		SetSlotInfo(SlotSwapInfo.GetTargetSlotIndex(), TargetSlotInfo);

		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.SetFromLast(SlotSwapInfo); 
		SetSlotInfo(SlotSwapInfo.GetLastSlotIndex(), LastSlotInfo);
	}
	else if (SlotSwapInfo.LastSlotData && !SlotSwapInfo.LastSlotComponent->GetRemoveItemsOnDrag())
	{
		UGISItemBaseWidget* itemTemp = GetItemWidget(SlotSwapInfo.GetTargetSlotIndex());
		itemTemp->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGISContainerBaseWidget::Widget_OnTabVisibilityChanged(int32 TabIndex)
{
}

void UGISContainerBaseWidget::Widget_OnTabChanged(int32 TabIndexIn)
{
	// 1. Remove all item child widgets in slots.
	for (UGISSlotBaseWidget* slot : InventoryTabs[TabIndexIn]->InventorySlots)
	{
		UGISItemBaseWidget* itemTemp = slot->ItemInSlot;
		itemTemp->SetVisibility(ESlateVisibility::Collapsed);
	}
	TArray<FGISSlotInfo> Slots = InventoryComponent->GetInventorySlots(TabIndexIn);
	//2. now, we will simply reconstruct all items, from new data in inventory.
	for (const FGISSlotInfo& slot : Slots)
	{
		if (slot.ItemData && ItemClass)
		{
			UGISItemBaseWidget* ItemWidget = GetItemWidget(FGISSlotIndexInfo(TabIndexIn, slot.SlotIndex));

			FGISSlotInfo SlotInfo = slot;
			SetSlotInfo(FGISSlotIndexInfo(TabIndexIn, slot.SlotIndex), SlotInfo);
			UpdateItemWidget(ItemWidget, SlotInfo);
		}
	}
}

UGISTabBaseWidget* UGISContainerBaseWidget::GetTabByName(FName TabNameIn)
{
	for (UGISTabBaseWidget* tabIt : InventoryTabs)
	{
		if (tabIt->TabInfo.TabName == TabNameIn)
		{
			return tabIt;
		}
	}
	return nullptr;
}

int32 UGISContainerBaseWidget::GetItemCount(int32 TabIndex)
{
	return InventoryTabs[TabIndex]->ItemCount;
}
void UGISContainerBaseWidget::IncrementItemCount(int32 TabIndex)
{
	InventoryTabs[TabIndex]->ItemCount++;
}
void UGISContainerBaseWidget::SetSlotInfo(const FGISSlotIndexInfo& Index, const FGISSlotInfo& SlotInfo)
{
	InventoryTabs[Index.TabIndex]->InventorySlots[Index.SlotIndex]->SlotInfo = SlotInfo;
}

void UGISContainerBaseWidget::SetSlotData(const FGISSlotIndexInfo& Index, class UGISItemData* DataIn)
{
	InventoryTabs[Index.TabIndex]->InventorySlots[Index.SlotIndex]->SlotInfo.ItemData = DataIn;
}
void UGISContainerBaseWidget::UpdateItemWidget(class UGISItemBaseWidget* Item, const FGISSlotInfo& SlotInfo)
{
	Item->ItemData = SlotInfo.ItemData;
	Item->InventoryComponent = InventoryComponent;
	Item->InitializeItem();
	Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
class UGISItemBaseWidget* UGISContainerBaseWidget::GetItemWidget(const FGISSlotIndexInfo& Index)
{
	return InventoryTabs[Index.TabIndex]->InventorySlots[Index.SlotIndex]->ItemInSlot;
}