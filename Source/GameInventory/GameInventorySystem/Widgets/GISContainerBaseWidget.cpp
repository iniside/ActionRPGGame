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
					if (Tab.LinkedTab < 0)
					{
						tabWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					}

					for (const FGISSlotInfo& Slot : Tab.TabSlots)
					{
						UGISSlotBaseWidget* slotWidget = ConstructObject<UGISSlotBaseWidget>(SlotClass, this);
						if (slotWidget)
						{
							ULocalPlayer* Player = WorldIn->GetFirstLocalPlayerFromController(); //temporary
							slotWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
							slotWidget->Initialize();
							slotWidget->SlotInfo = Slot;
							slotWidget->GISItemClass = ItemClass;
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
	{//check against replicated component and struct in tabs in it.
		UGISItemBaseWidget* ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass, Outer);
		if (ItemWidget && InventoryComponent)
		{
			ULocalPlayer* Player = InventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
			ItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
			ItemWidget->Initialize();
			ItemWidget->ItemData = SlotUpdateInfo.SlotData;
			ItemWidget->InventoryComponent = InventoryComponent;
			ItemWidget->InitializeItem();
			//ItemWidget->LastSlotInfo = SlotInfo;
		}
		
		FGISSlotInfo SlotInfo;
		SlotInfo.CurrentInventoryComponent = SlotUpdateInfo.SlotComponent;
		SlotInfo.ItemData = SlotUpdateInfo.SlotData;
		SlotInfo.SlotIndex = SlotUpdateInfo.SlotIndex;
		SlotInfo.SlotTabIndex = SlotUpdateInfo.TabIndex;
		InventoryTabs[SlotUpdateInfo.TabIndex]->ItemCount++;
		InventoryTabs[SlotUpdateInfo.TabIndex]->InventorySlots[SlotUpdateInfo.SlotIndex]->SlotInfo = SlotInfo;

		UWidget* superWidget = InventoryTabs[SlotUpdateInfo.TabIndex]->InventorySlots[SlotUpdateInfo.SlotIndex]->GetWidgetFromName(DropSlottName);
		
		UOverlay* overlay = Cast<UOverlay>(superWidget);
		if (overlay)
		{
			int32 childCount = overlay->GetChildrenCount();
			if (childCount > 1)
			{
				overlay->RemoveChildAt(childCount - 1);
			}
			overlay->AddChild(ItemWidget);
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
	UGISContainerBaseWidget* awesomeTest = this;
	float lolo = 10;
}

void UGISContainerBaseWidget::AddItem(const FGISSlotSwapInfo& SlotSwapInfo)
{
	UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
	if (!SlotSwapInfo.LastSlotData)
	{
		UGISItemBaseWidget* ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass, Outer);
		if (ItemWidget && InventoryComponent)
		{
			ULocalPlayer* Player = InventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
			ItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
			ItemWidget->Initialize();
			ItemWidget->ItemData = SlotSwapInfo.TargetSlotData;
			ItemWidget->InventoryComponent = InventoryComponent;
			ItemWidget->InitializeItem();
			//ItemWidget->LastSlotInfo = SlotInfo;
		}
		FGISSlotInfo TargetSlotInfo;
		TargetSlotInfo.CurrentInventoryComponent = SlotSwapInfo.TargetSlotComponent;
		TargetSlotInfo.ItemData = SlotSwapInfo.TargetSlotData;
		TargetSlotInfo.SlotIndex = SlotSwapInfo.TargetSlotIndex;
		TargetSlotInfo.SlotTabIndex = SlotSwapInfo.TargetTabIndex;
		InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->SlotInfo = TargetSlotInfo;
		UWidget* superWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->GetWidgetFromName(DropSlottName);

		UOverlay* overlay = Cast<UOverlay>(superWidget);
		if (overlay)
		{
			overlay->AddChild(ItemWidget);
		}
	}
	else if (SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		//construct target and last, since this is for test one will do just as well.
		UGISItemBaseWidget* TargetItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass, Outer);
		if (TargetItemWidget && InventoryComponent)
		{
			ULocalPlayer* Player = InventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
			TargetItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
			TargetItemWidget->Initialize();
			TargetItemWidget->ItemData = SlotSwapInfo.TargetSlotData;
			TargetItemWidget->InventoryComponent = InventoryComponent;
			TargetItemWidget->InitializeItem();
			//ItemWidget->LastSlotInfo = SlotInfo;
		}

		UGISItemBaseWidget* LastItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass, Outer);
		if (TargetItemWidget && InventoryComponent)
		{
			ULocalPlayer* Player = InventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
			LastItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
			LastItemWidget->Initialize();
			LastItemWidget->ItemData = SlotSwapInfo.LastSlotData;
			LastItemWidget->InventoryComponent = InventoryComponent;
			LastItemWidget->InitializeItem();
			//ItemWidget->LastSlotInfo = SlotInfo;
		}
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

		UWidget* lastSlotWidget = InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->GetWidgetFromName(DropSlottName);
		UWidget* targetSlotWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->GetWidgetFromName(DropSlottName);
		
		UOverlay* lastSlotOverlay = Cast<UOverlay>(lastSlotWidget);
		if (lastSlotOverlay)
		{
			lastSlotOverlay->AddChild(LastItemWidget);
		}

		UOverlay* targetSlotOverlay = Cast<UOverlay>(targetSlotWidget);
		if (targetSlotOverlay)
		{
			targetSlotOverlay->AddChild(TargetItemWidget);
		}
	}
}
void UGISContainerBaseWidget::RemoveItem(const FGISSlotSwapInfo& SlotSwapInfo)
{
	if (!SlotSwapInfo.LastSlotData && SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		UWidget* superWidget = InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->GetWidgetFromName(DropSlottName);

		//this it bit fiddly since the widget which will contain our widget must be last
		//and out item widget must be last child within this widget
		UOverlay* overlay = Cast<UOverlay>(superWidget);
		if (overlay)
		{
			int32 childCount = overlay->GetChildrenCount();
			if (childCount > 1)
			{
				overlay->RemoveChildAt(childCount - 1);
			}
		}
	}
	else if (SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		UWidget* lastSlotWidget = InventoryTabs[SlotSwapInfo.LastTabIndex]->InventorySlots[SlotSwapInfo.LastSlotIndex]->GetWidgetFromName(DropSlottName);
		UWidget* targetSlotWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->GetWidgetFromName(DropSlottName);
		//this it bit fiddly since the widget which will contain our widget must be last
		//and out item widget must be last child within this widget
		UOverlay* lastSlotOverlay = Cast<UOverlay>(lastSlotWidget);
		if (lastSlotOverlay)
		{
			int32 childCount = lastSlotOverlay->GetChildrenCount();
			if (childCount > 1)
			{
				lastSlotOverlay->RemoveChildAt(childCount - 1);
			}
		}
		UOverlay* targetSlotOverlay = Cast<UOverlay>(targetSlotWidget);
		if (lastSlotOverlay)
		{
			int32 childCount = targetSlotOverlay->GetChildrenCount();
			if (childCount > 1)
			{
				targetSlotOverlay->RemoveChildAt(childCount - 1);
			}
		}
	}
	else if (SlotSwapInfo.LastSlotData && !SlotSwapInfo.LastSlotComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		UWidget* superWidget = InventoryTabs[SlotSwapInfo.TargetTabIndex]->InventorySlots[SlotSwapInfo.TargetSlotIndex]->GetWidgetFromName(DropSlottName);

		//this it bit fiddly since the widget which will contain our widget must be last
		//and out item widget must be last child within this widget
		UOverlay* overlay = Cast<UOverlay>(superWidget);
		if (overlay)
		{
			int32 childCount = overlay->GetChildrenCount();
			if (childCount > 1)
			{
				overlay->RemoveChildAt(childCount - 1);
			}
		}
	}
}

void UGISContainerBaseWidget::Widget_OnTabVisibilityChanged(int32 TabIndex)
{
	if (InventoryComponent->Tabs.InventoryTabs[TabIndex].bIsTabVisible)
	{
		InventoryTabs[TabIndex]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		InventoryTabs[TabIndex]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGISContainerBaseWidget::Widget_OnTabChanged(int32 TabIndexIn)
{
	UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
	//	UGISItemData* itemData = SlotUpdateInfo.SlotComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].TabSlots[SlotUpdateInfo.SlotIndex].ItemData;
	// 1. Remove all item child widgets in slots.
	for (UGISSlotBaseWidget* slot : InventoryTabs[TabIndexIn]->InventorySlots)
	{
		UWidget* superWidget = slot->GetWidgetFromName(DropSlottName);

		UOverlay* overlay = Cast<UOverlay>(superWidget);
		if (overlay)
		{
			int32 childCount = overlay->GetChildrenCount();
			if (childCount > 1)
			{
				overlay->RemoveChildAt(childCount - 1);
			}
		}
	}

	//2. now, we will simply reconstruct all items, from new data in inventory.
	for (const FGISSlotInfo& slot : InventoryComponent->Tabs.InventoryTabs[TabIndexIn].TabSlots)
	{
		if (slot.ItemData && ItemClass)
		{
			UGISItemBaseWidget* ItemWidget = ConstructObject<UGISItemBaseWidget>(ItemClass, Outer);
			if (ItemWidget && InventoryComponent)
			{
				ULocalPlayer* Player = InventoryComponent->GetWorld()->GetFirstLocalPlayerFromController(); //temporary
				ItemWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
				ItemWidget->Initialize();
				ItemWidget->ItemData = slot.ItemData;
				ItemWidget->InventoryComponent = InventoryComponent;
				ItemWidget->InitializeItem();
				//ItemWidget->LastSlotInfo = SlotInfo;
			}

			FGISSlotInfo SlotInfo;
			SlotInfo.CurrentInventoryComponent = slot.CurrentInventoryComponent;
			SlotInfo.ItemData = slot.ItemData;
			SlotInfo.SlotIndex = slot.SlotIndex;
			SlotInfo.SlotTabIndex = slot.SlotTabIndex;
			InventoryTabs[TabIndexIn]->InventorySlots[slot.SlotIndex]->SlotInfo = SlotInfo;

			UWidget* superWidget = InventoryTabs[TabIndexIn]->InventorySlots[slot.SlotIndex]->GetWidgetFromName(DropSlottName);

			UOverlay* overlay = Cast<UOverlay>(superWidget);
			if (overlay)
			{
				overlay->AddChild(ItemWidget);
			}
		}
	}

	//haha let's hope it will work.

}