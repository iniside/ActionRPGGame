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

void UGISContainerBaseWidget::InitializeContainer()
{
	if (InventoryComponent)
	{
		//bind functions to delegates:
		InventoryComponent->OnInventoryLoaded.AddDynamic(this, &UGISContainerBaseWidget::InitializeInventory);
		InventoryComponent->OnItemAdded.AddDynamic(this, &UGISContainerBaseWidget::Widget_OnItemAdded);
		InventoryComponent->OnItemSlotSwapped.AddDynamic(this, &UGISContainerBaseWidget::Widget_OnItemSlotSwapped);
		InventoryComponent->OnTabVisibilityChanged.BindUObject(this, &UGISContainerBaseWidget::Widget_OnTabVisibilityChanged);
		InventoryComponent->OnTabChanged.BindUObject(this, &UGISContainerBaseWidget::Widget_OnTabChanged);
		//InitializeInventory();
	}
}
void UGISContainerBaseWidget::InitializeInventory()
{
	if (InventoryComponent && InventoryTabs.Num() == 0)
	{
		UWorld* WorldIn = InventoryComponent->GetWorld();
		int32 IndexCounter = 0;
		UObject* Outer = WorldIn->GetGameInstance() ? StaticCast<UObject*>(WorldIn->GetGameInstance()) : StaticCast<UObject*>(WorldIn);
		for (const FGISTabInfo& Tab : InventoryComponent->Tabs.InventoryTabs)
		{
			if (TabClass && SlotClass)
			{

				UGISTabBaseWidget* tabWidget = ConstructObject<UGISTabBaseWidget>(TabClass, Outer);
				if (tabWidget)
				{
					ULocalPlayer* Player = WorldIn->GetFirstLocalPlayerFromController(); //temporary
					tabWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
					tabWidget->Initialize();
					tabWidget->TabInfo = Tab;
					//tabWidget->SetVisibility(ESlateVisibility::Hidden);
					for (const FGISSlotInfo& Slot : Tab.TabSlots)
					{
						UGISSlotBaseWidget* slotWidget = ConstructObject<UGISSlotBaseWidget>(SlotClass, this);
						if (slotWidget)
						{
							ULocalPlayer* Player = WorldIn->GetFirstLocalPlayerFromController(); //temporary
							slotWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
							slotWidget->Initialize();
							slotWidget->SlotInfo = Slot;
							slotWidget->TabInfo = Tab;
							slotWidget->DropSlottName = DropSlottName;
							slotWidget->GISItemClass = ItemClass;

							UGISItemBaseWidget* ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
							ItemWidget->Initialize();
							ItemWidget->ItemData = nullptr;
							ItemWidget->InventoryComponent = InventoryComponent;
							ItemWidget->InitializeItem();
							ItemWidget->SetVisibility(ESlateVisibility::Collapsed);
							UWidget* superWidget = slotWidget->GetWidgetFromName(DropSlottName);
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
}
void UGISContainerBaseWidget::Widget_OnItemAdded(const FGISSlotUpdateData& SlotUpdateInfo)
{
	UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());

	/*
		if Item count between widget and actual inventory is different
		it means, that to inventory on server multiple items have been added
		and we need to reconstruct all slots.
	*/
	if (InventoryComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].ItemCount
		!= InventoryTabs[SlotUpdateInfo.TabIndex]->ItemCount)
	{
		Widget_OnTabChanged(SlotUpdateInfo.TabIndex);
	}

//	UGISItemData* itemData = SlotUpdateInfo.SlotComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].TabSlots[SlotUpdateInfo.SlotIndex].ItemData;
	if (SlotUpdateInfo.SlotData && ItemClass)
	{

		FGISSlotInfo SlotInfo;
		SlotInfo.CurrentInventoryComponent = SlotUpdateInfo.SlotComponent;
		SlotInfo.ItemData = SlotUpdateInfo.SlotData;
		SlotInfo.SlotIndex = SlotUpdateInfo.SlotIndex;
		SlotInfo.SlotTabIndex = SlotUpdateInfo.TabIndex;
		InventoryTabs[SlotUpdateInfo.TabIndex]->ItemCount++;
		InventoryTabs[SlotUpdateInfo.TabIndex]->InventorySlots[SlotUpdateInfo.SlotIndex]->SlotInfo = SlotInfo;

		UGISItemBaseWidget* ItemWidget = InventoryTabs[SlotUpdateInfo.TabIndex]->InventorySlots[SlotUpdateInfo.SlotIndex]->ItemInSlot;
		if (InventoryComponent)
		{
			if (!ItemWidget)
			{
				ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
			}
			ItemWidget->Initialize();
			ItemWidget->ItemData = SlotUpdateInfo.SlotData;
			ItemWidget->InventoryComponent = InventoryComponent;
			ItemWidget->InitializeItem();
			ItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
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
			&& SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
		{
			//actually, probabaly need separate functions, as there might be more
			//complex scases like actuall swapping items, instead of puting it
			//in empty slot in another component.
			SlotSwapInfo.LastSlotComponent->InventoryContainer->RemoveItem(SlotSwapInfo);
			SlotSwapInfo.TargetSlotComponent->InventoryContainer->AddItem(SlotSwapInfo);
		}
		else if (!SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
		{
			SlotSwapInfo.TargetSlotComponent->InventoryContainer->RemoveItem(SlotSwapInfo);
			SlotSwapInfo.TargetSlotComponent->InventoryContainer->AddItem(SlotSwapInfo);
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
		TargetSlotInfo.CurrentInventoryComponent = SlotSwapInfo.TargetSlotComponent;
		TargetSlotInfo.ItemData = SlotSwapInfo.TargetSlotData;
		TargetSlotInfo.SlotIndex = SlotSwapInfo.TargetSlotIndex;
		TargetSlotInfo.SlotTabIndex = SlotSwapInfo.TargetTabIndex;
		InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->SlotInfo = TargetSlotInfo;

		UGISItemBaseWidget* ItemWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->ItemInSlot;
		if (InventoryComponent)
		{
			if (!ItemWidget)
			{
				ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
			}
			ItemWidget->Initialize();
			ItemWidget->ItemData = TargetSlotInfo.ItemData;
			ItemWidget->InventoryComponent = InventoryComponent;
			ItemWidget->InitializeItem();
			ItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else if (SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{

		
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.CurrentInventoryComponent = SlotSwapInfo.TargetSlotComponent;
		TargetSlotInfo.ItemData = SlotSwapInfo.TargetSlotData;
		TargetSlotInfo.SlotIndex = SlotSwapInfo.TargetSlotIndex;
		TargetSlotInfo.SlotTabIndex = SlotSwapInfo.TargetTabIndex;
		InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->SlotInfo.ItemData = SlotSwapInfo.TargetSlotData;

		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.CurrentInventoryComponent = SlotSwapInfo.LastSlotComponent;
		LastSlotInfo.ItemData = SlotSwapInfo.LastSlotData;
		LastSlotInfo.SlotIndex = SlotSwapInfo.LastSlotIndex;
		LastSlotInfo.SlotTabIndex = SlotSwapInfo.LastTabIndex;
		SlotSwapInfo.LastSlotComponent->InventoryContainer->InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->SlotInfo.ItemData = SlotSwapInfo.LastSlotData;
		
		UGISItemBaseWidget* TargetItemWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->ItemInSlot;
		if (InventoryComponent)
		{
			if (!TargetItemWidget)
			{
				TargetItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
			}
			TargetItemWidget->Initialize();
			TargetItemWidget->ItemData = SlotSwapInfo.TargetSlotData;
			TargetItemWidget->InventoryComponent = InventoryComponent;
			TargetItemWidget->InitializeItem();
			TargetItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		UGISItemBaseWidget* LastItemWidget = SlotSwapInfo.LastSlotComponent->InventoryContainer->InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->ItemInSlot;
		if (InventoryComponent)
		{
			if (!LastItemWidget)
			{
				LastItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
			}
			LastItemWidget->Initialize();
			LastItemWidget->ItemData = SlotSwapInfo.LastSlotData;
			LastItemWidget->InventoryComponent = SlotSwapInfo.LastSlotComponent.Get();
			LastItemWidget->InitializeItem();
			LastItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else
	{
		//construct target and last, since this is for test one will do just as well.
		UGISItemBaseWidget* TargetItemWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->ItemInSlot;
		if (InventoryComponent)
		{
			if (!TargetItemWidget)
			{
				TargetItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass);
			}
			TargetItemWidget->Initialize();
			TargetItemWidget->ItemData = SlotSwapInfo.TargetSlotData;
			TargetItemWidget->InventoryComponent = InventoryComponent;
			TargetItemWidget->InitializeItem();
			TargetItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.CurrentInventoryComponent = SlotSwapInfo.TargetSlotComponent;
		TargetSlotInfo.ItemData = SlotSwapInfo.TargetSlotData;
		TargetSlotInfo.SlotIndex = SlotSwapInfo.TargetSlotIndex;
		TargetSlotInfo.SlotTabIndex = SlotSwapInfo.TargetTabIndex;
		InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->SlotInfo = TargetSlotInfo;
	}
}
void UGISContainerBaseWidget::RemoveItem(const FGISSlotSwapInfo& SlotSwapInfo)
{
	if (!SlotSwapInfo.LastSlotData && SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		//InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->bIsEnabled = false;
		//this it bit fiddly since the widget which will contain our widget must be last
		//and out item widget must be last child within this widget
		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.CurrentInventoryComponent = SlotSwapInfo.LastSlotComponent;
		LastSlotInfo.ItemData = SlotSwapInfo.LastSlotData;
		LastSlotInfo.SlotIndex = SlotSwapInfo.LastSlotIndex;
		LastSlotInfo.SlotTabIndex = SlotSwapInfo.LastTabIndex;

		InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->SlotInfo = LastSlotInfo;
		
		UGISItemBaseWidget* itemTemp = InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->ItemInSlot;
		itemTemp->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		UGISItemBaseWidget* LastItem = InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->ItemInSlot;
		LastItem->SetVisibility(ESlateVisibility::Collapsed);

		UGISItemBaseWidget* TargetITem = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->ItemInSlot;
		TargetITem->SetVisibility(ESlateVisibility::Collapsed);

		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.CurrentInventoryComponent = SlotSwapInfo.TargetSlotComponent;
		TargetSlotInfo.ItemData = SlotSwapInfo.TargetSlotData;
		TargetSlotInfo.SlotIndex = SlotSwapInfo.TargetSlotIndex;
		TargetSlotInfo.SlotTabIndex = SlotSwapInfo.TargetTabIndex;
		InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->SlotInfo = TargetSlotInfo;

		FGISSlotInfo LastSlotInfo;
		LastSlotInfo.CurrentInventoryComponent = SlotSwapInfo.LastSlotComponent;
		LastSlotInfo.ItemData = SlotSwapInfo.LastSlotData;
		LastSlotInfo.SlotIndex = SlotSwapInfo.LastSlotIndex;
		LastSlotInfo.SlotTabIndex = SlotSwapInfo.LastTabIndex;
		InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->SlotInfo = LastSlotInfo;
	}
	else if (SlotSwapInfo.LastSlotData && !SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		UGISItemBaseWidget* itemTemp = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->ItemInSlot;
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

	//2. now, we will simply reconstruct all items, from new data in inventory.
	for (const FGISSlotInfo& slot : InventoryComponent->Tabs.InventoryTabs[TabIndexIn].TabSlots)
	{
		if (slot.ItemData && ItemClass)
		{
			UGISItemBaseWidget* ItemWidget = InventoryTabs[TabIndexIn]->InventorySlots[slot.SlotIndex]->ItemInSlot;

			FGISSlotInfo SlotInfo;
			SlotInfo.CurrentInventoryComponent = slot.CurrentInventoryComponent;
			SlotInfo.ItemData = slot.ItemData;
			SlotInfo.SlotIndex = slot.SlotIndex;
			SlotInfo.SlotTabIndex = slot.SlotTabIndex;
			InventoryTabs[TabIndexIn]->InventorySlots[slot.SlotIndex]->SlotInfo = SlotInfo;

			if (InventoryComponent)
			{
				ItemWidget->Initialize();
				ItemWidget->ItemData = slot.ItemData;
				ItemWidget->InventoryComponent = InventoryComponent;
				ItemWidget->InitializeItem();
				ItemWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				//ItemWidget->LastSlotInfo = SlotInfo;
			}
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