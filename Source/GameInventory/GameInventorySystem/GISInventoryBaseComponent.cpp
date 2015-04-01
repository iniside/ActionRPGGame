// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "GISGlobalTypes.h"

#include "GISItemData.h"
#include "IGISPickupItem.h"
#include "GISPickupActor.h"

#include "Widgets/GISContainerBaseWidget.h"
#include "Widgets/GISLootContainerBaseWidget.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "GISInventoryBaseComponent.h"

UGISInventoryBaseComponent::UGISInventoryBaseComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bAutoRegister = true;
	bReplicateTabsToOwnerOnly = true;
	bRemoveItemsFromInvetoryOnDrag = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bRunOnAnyThread = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAllowConcurrentTick = true;
	InventoryVisibility = ESlateVisibility::Hidden;
	LootWindowVisibility = ESlateVisibility::Hidden;

	LastTargetTab = INDEX_NONE;
	LastOtherOriginTab = INDEX_NONE;
	LastOtherOriginInventory = nullptr;
}


void UGISInventoryBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ENetRole CurrentRole = GetOwnerRole();
	ENetMode CurrentNetMode = GetNetMode();

	//PrimaryComponentTick.bStartWithTickEnabled = bTickInventory;
	if (CurrentRole == ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
	{
		InitializeInventoryTabs();
	}
			

}

void UGISInventoryBaseComponent::InitializeWidgets(APlayerController* PCIn)
{
	ENetRole CurrentRole = GetOwnerRole();
	ENetMode CurrentNetMode = GetNetMode();
	
	if (CurrentRole < ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
	{
		if (InventoryConfiguration.IsValid())
		{
			InventoryContainer = CreateWidget<UGISContainerBaseWidget>(PCIn, InventoryConfiguration.InventoryContainerClass);
			if (InventoryContainer)
			{
				InventoryContainer->InitializeContainer(InventoryConfiguration, this, PCIn);
				InventoryContainer->SetVisibility(InventoryVisibility);
				//call last
			}
		}

		if (LootConfiguration.IsValid())
		{
			LootWidget = CreateWidget<UGISLootContainerBaseWidget>(PCIn, LootConfiguration.LootWidgetClass);
			if (LootWidget)
			{
				LootWidget->InitializeLootWidget(LootConfiguration, this, PCIn);
				LootWidget->SetVisibility(LootWindowVisibility);
			}
		}
	}
}

void UGISInventoryBaseComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentPickupActor)
	{
		float distance = FVector::Dist(CurrentPickupActor->GetActorLocation(), GetOwner()->GetActorLocation());
		if (distance > MaxLootingDistance)
		{
			ClientHideLootingWidget();
			SetComponentTickEnabled(false);
		}
	}
}
void UGISInventoryBaseComponent::OnRep_InventoryCreated()
{
	OnInventoryLoaded.Broadcast();
}

void UGISInventoryBaseComponent::OnRep_SlotUpdate()
{
	OnItemAdded.Broadcast(SlotUpdateInfo);
}

void UGISInventoryBaseComponent::OnRep_SlotSwap()
{
	OnItemSlotSwapped.Broadcast(SlotSwapInfo);
}
void UGISInventoryBaseComponent::PostInitProperties()
{
	Super::PostInitProperties();
	
}

FGISInventoryTab UGISInventoryBaseComponent::GetInventory()
{
	return Tabs;
}

void UGISInventoryBaseComponent::PickItem(AActor* PickupItemIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerPickItem(PickupItemIn);
	}
	else
	{
		IIGISPickupItem* pickupItem = Cast<IIGISPickupItem>(PickupItemIn);
		if (pickupItem)
		{
			//do something, I'm not sure what yet.
		}
	}
}

void UGISInventoryBaseComponent::ServerPickItem_Implementation(AActor* PickupItemIn)
{
	PickItem(PickupItemIn);
}
bool UGISInventoryBaseComponent::ServerPickItem_Validate(AActor* PickupItemIn)
{
	return true;
}
void UGISInventoryBaseComponent::AddItemToInventory(class UGISItemData* ItemIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerAddItemToInventory(ItemIn);
	}
	else
	{
		//add item to first empty slot in first matching tab.
		for (FGISTabInfo& TabInfo : Tabs.InventoryTabs)
		{
			for (FGISSlotInfo& Slot : TabInfo.TabSlots)
			{
				if (Slot.ItemData == nullptr)
				{
					Slot.ItemData = ItemIn;
					ItemIn->CurrentInventory = this;
					TabInfo.ItemCount++;
					//Slot.ItemData->OnItemRemovedFromSlot();
					SlotUpdateInfo.TabIndex = TabInfo.TabIndex;
					SlotUpdateInfo.SlotIndex = Slot.SlotIndex;
					SlotUpdateInfo.SlotData = Slot.ItemData;
					SlotUpdateInfo.SlotComponent = this;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnItemAdded.Broadcast(SlotUpdateInfo);
					return;
				}
			}
		}
	}
}

void UGISInventoryBaseComponent::ServerAddItemToInventory_Implementation(class UGISItemData* ItemIn)
{
	AddItemToInventory(ItemIn);
}
bool UGISInventoryBaseComponent::ServerAddItemToInventory_Validate(class UGISItemData* ItemIn)
{
	return true;
}

void UGISInventoryBaseComponent::AddItemOnSlot(const FGISSlotInfo& TargetSlotType, const FGISSlotInfo& LastSlotType)
{
	//Before we start swapping item, let's check if tags match!
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerAddItemOnSlot(TargetSlotType, LastSlotType);
		return;
	}
	FGISSlotInfo& LastSlot = const_cast<FGISSlotInfo&>(LastSlotType);
	FGISSlotInfo& TargetSlot = const_cast<FGISSlotInfo&>(TargetSlotType);
	//check if all data is valid.
	if (!TargetSlot.IsValid() || !LastSlot.IsValid())
		return;

	//next check should be against item tags, but that's later!
	if (LastSlotType.CurrentInventoryComponent->bRemoveItemsFromInvetoryOnDrag)
	{
		//Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotTabIndex].ItemData
		if (TargetSlot.GetItemData() == nullptr)
		{
			UGISItemData* TargetItem = LastSlot.GetItemData(); 
			UGISItemData* LastItem = TargetSlot.GetItemData();
			if (!CheckIfCanAddItemToSlot(TargetItem, TargetSlotType.SlotTabIndex, TargetSlotType.SlotIndex, LastItem))
				return;

			if (!TargetItem->HasAnyMatchingGameplayTags(TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].Tags))
				return;

			if (TargetItem)
				if (!TargetItem->CanItemBeSwapped())
					return;
			if (LastItem)
				if (!LastItem->CanItemBeSwapped())
					return;

			TargetItem->CurrentInventory = TargetSlot.CurrentInventoryComponent.Get(); //seems approperties instead of this ?
			TargetItem->LastInventory = LastSlot.CurrentInventoryComponent.Get();

			LastSlot.DecrementItemCount();
			TargetSlot.IncrementItemCount(); 

			TargetSlot.SetItemData(TargetItem);
			LastSlot.SetItemData(nullptr); 
			
			if ((LastTargetTab != INDEX_NONE) && (LastOtherOriginTab != INDEX_NONE)
				&& LastOtherOriginInventory)
			{
				LastOtherOriginInventory->CopyItemsFromOtherInventoryTab(this, LastOtherOriginTab, LastTargetTab);
			}

			TargetItem->SetWorld(GetWorld());
			TargetItem->SetCurrentOwner(GetOwner());
			OnItemAddedToSlot(TargetItem);

			SlotSwapInfo.ReplicationCounter++;
			SlotSwapInfo = FGISSlotSwapInfo(LastSlot, LastItem, TargetSlot, TargetItem);

			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
				LastSlotType.CurrentInventoryComponent->OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			}
			//we will do it on server, to give chance for other
			//componenets/actors to do something if item is removed/swapped in inventory.
			if (GetNetMode() == ENetMode::NM_DedicatedServer)
			{
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
				LastSlotType.CurrentInventoryComponent->OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			}
		}
		else
		{

			/*
				Last copy item from last inventory, to be placed in Target inventory.
			*/
			UGISItemData* TargetItem = LastSlot.GetItemData();
			/*
				Copy item from target inventory, to be placed in last inventory.
			*/
			UGISItemData* LastItem = TargetSlot.GetItemData();
			
			if (!CheckIfCanAddItemToSlot(TargetItem, TargetSlotType.SlotTabIndex, TargetSlotType.SlotIndex, LastItem))
				return;

			if (!TargetItem->HasAnyMatchingGameplayTags(TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].Tags))
				return;
			
			if (!LastItem->HasAnyMatchingGameplayTags(LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].Tags))
				return;

			if (TargetItem)
				if (!TargetItem->CanItemBeSwapped())
					return;
			if (LastItem)
				if (!LastItem->CanItemBeSwapped())
					return;

			/*
				Assign current inventory to current item.
			*/
			TargetItem->CurrentInventory = TargetSlotType.CurrentInventoryComponent.Get(); //seems approperties instead of this ?
			/*
				Assign last inventory to current item.
			*/
			TargetItem->LastInventory = LastSlotType.CurrentInventoryComponent.Get();
			/*
				Assign last inventory to last item, as current inventory, since it's
				possibly coming from different inventory than Target.
			*/
			LastItem->CurrentInventory = LastSlotType.CurrentInventoryComponent.Get();
			/*
				Same as above just in reverse. Last item is coming from TargetSlot, so we assign
				Last Inventory from TargetSlot.
			*/
			LastItem->LastInventory = TargetSlotType.CurrentInventoryComponent.Get();
			
			LastSlot.DecrementItemCount();
			TargetSlot.IncrementItemCount();

			TargetSlot.SetItemData(TargetItem);
			LastSlot.SetItemData(LastItem);

			if ((LastTargetTab != INDEX_NONE) && (LastOtherOriginTab != INDEX_NONE)
				&& LastOtherOriginInventory)
			{
				LastOtherOriginInventory->CopyItemsFromOtherInventoryTab(this, LastOtherOriginTab, LastTargetTab);
			}

			TargetItem->SetWorld(GetWorld());
			TargetItem->SetCurrentOwner(GetOwner());
			LastItem->SetWorld(GetWorld());
			LastItem->SetCurrentOwner(GetOwner());

			OnItemAddedToSlot(TargetItem);
			OnItemAddedToSlot(LastItem);

			SlotSwapInfo.ReplicationCounter++;
			SlotSwapInfo = FGISSlotSwapInfo(LastSlot, LastItem, TargetSlot, TargetItem);
	
			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
				LastSlotType.CurrentInventoryComponent->OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			}
			//we will do it on server, to give chance for other
			//componenets/actors to do something if item is removed/swapped in inventory.
			if (GetNetMode() == ENetMode::NM_DedicatedServer)
			{
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
				LastSlotType.CurrentInventoryComponent->OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			}
				
			//ClientSlotSwap(SlotSwapInfo);
		}
	}
	else
	{ //if there is item in target slot. Remove it. or override it.
		UGISItemData* TargetItem = LastSlot.GetItemData();
		UGISItemData* LastItem = TargetSlot.GetItemData(); //Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData;
		
		if (TargetItem)
			if (!TargetItem->CanItemBeSwapped())
				return;
		if (LastItem)
			if (!LastItem->CanItemBeSwapped())
				return;
		
		LastSlot.SetItemData(nullptr);
		TargetSlot.SetItemData(TargetItem);

		TargetItem->CurrentInventory = TargetSlotType.CurrentInventoryComponent.Get();

		if ((LastTargetTab != INDEX_NONE) && (LastOtherOriginTab != INDEX_NONE)
			&& LastOtherOriginInventory)
		{
			LastOtherOriginInventory->CopyItemsFromOtherInventoryTab(this, LastOtherOriginTab, LastTargetTab);
		}

		TargetItem->SetWorld(GetWorld());
		TargetItem->SetCurrentOwner(GetOwner());

		OnItemAddedToSlot(TargetItem);

		SlotSwapInfo.ReplicationCounter++;
		SlotSwapInfo = FGISSlotSwapInfo(LastSlot, LastItem, TargetSlot, TargetItem);
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnItemSlotSwapped.Broadcast(SlotSwapInfo);
	}
}
void UGISInventoryBaseComponent::ServerAddItemOnSlot_Implementation(const FGISSlotInfo& TargetSlotType, const FGISSlotInfo& LastSlotType)
{
	AddItemOnSlot(TargetSlotType, LastSlotType);
}
bool UGISInventoryBaseComponent::ServerAddItemOnSlot_Validate(const FGISSlotInfo& TargetSlotType, const FGISSlotInfo& LastSlotType)
{
	return true;
}
bool UGISInventoryBaseComponent::CheckIfCanAddItemToSlot(class UGISItemData* TargetDataIn, int32 TargetTabIndex, int32 TargetSlotIndex, 
		class UGISItemData* LastDataIn)
{
	return true;
}
void UGISInventoryBaseComponent::OnItemAddedToSlot(class UGISItemData* AddedItemIn)
{
	if (AddedItemIn)
	{
		AddedItemIn->OnItemRemovedFromSlot();
		AddedItemIn->OnItemAddedToSlot();
	}
}


void UGISInventoryBaseComponent::RemoveItem(const FGISSlotInfo& TargetSlotType)
{

}


void UGISInventoryBaseComponent::OnRep_LootedItems()
{
	OnLootingStart.Broadcast();
}
void UGISInventoryBaseComponent::OnRep_PickupActor()
{
}

void UGISInventoryBaseComponent::StartLooting(class AGISPickupActor* PickUp)
{
	int32 ItemNum = PickUp->ItemToLoot.Num();
	CurrentPickupActor = PickUp;
	LootFromPickup.Loot.Empty();
	for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
	{
		FGISLootSlotInfo lootInfo;
		lootInfo.OwningPickupActor = PickUp;
		lootInfo.SlotComponent = this;
		lootInfo.SlotIndex = ItemIndex;
		LootFromPickup.Loot.Add(lootInfo);
		LootFromPickup.ForceRep++;
	}
}

void UGISInventoryBaseComponent::LootOneItem(int32 ItemIndex)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		CurrentPickupActor->ItemToLoot.RemoveAtSwap(ItemIndex, 1, false);
		SeverLootOneItem(ItemIndex);
	}
	else
	{
		if (CurrentPickupActor)
		{
			//if it is not valid index both arrays, then something is wrong.
			if (LootFromPickup.Loot.IsValidIndex(ItemIndex) && CurrentPickupActor->ItemToLoot.IsValidIndex(ItemIndex))
			{
				UGISItemData* dataDuplicate = ConstructObject<UGISItemData>(CurrentPickupActor->ItemToLoot[ItemIndex]->GetClass(), this, NAME_None, RF_NoFlags, CurrentPickupActor->ItemToLoot[ItemIndex]);
				AddItemToInventory(dataDuplicate);
				//ok we removed one item. We need to rconstruct widgets, indexes etc, to make sure arry
				//have proper indexes in first place.
				//LootFromPickup.Loot.RemoveAtSwap(ItemIndex, 1, false);
				CurrentPickupActor->ItemToLoot.RemoveAtSwap(ItemIndex, 1, false);
				int32 ItemNum = CurrentPickupActor->ItemToLoot.Num();
				LootFromPickup.Loot.Empty();
				for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
				{
					FGISLootSlotInfo lootInfo;
					lootInfo.OwningPickupActor = CurrentPickupActor;
					lootInfo.SlotComponent = this;
					lootInfo.SlotIndex = ItemIndex;
					LootFromPickup.Loot.Add(lootInfo);
				}
				LootFromPickup.ForceRep++;
			}
			//reconstruct widget.
			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				OnRep_LootedItems();
			}
		}
	}
}
void UGISInventoryBaseComponent::SeverLootOneItem_Implementation(int32 ItemIndex)
{
	LootOneItem(ItemIndex);
}
bool UGISInventoryBaseComponent::SeverLootOneItem_Validate(int32 ItemIndex)
{
	return true;
}

void UGISInventoryBaseComponent::LootItems()
{
	//if (GetOwnerRole() < ROLE_Authority)
	//{
	//	ServerLootItems();
	//}
	//else
	//{
	//	if (!CurrentPickupActor)
	//		return;


	//	for (UGISItemData* Item : LootedItems)
	//	{
	//		AddItemToInventory(Item);
	//	}
	//}
}
void UGISInventoryBaseComponent::ServerLootItems_Implementation()
{
	LootItems();
}
bool UGISInventoryBaseComponent::ServerLootItems_Validate()
{
	return true;
}
void UGISInventoryBaseComponent::DropItemFromInventory(const FGISItemDropInfo& DropItemInfoIn)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerDropItemFromInventory(DropItemInfoIn);
	}
	else
	{

	}
}
void UGISInventoryBaseComponent::ServerDropItemFromInventory_Implementation(const FGISItemDropInfo& DropItemInfoIn)
{
	DropItemFromInventory(DropItemInfoIn);
}
bool UGISInventoryBaseComponent::ServerDropItemFromInventory_Validate(const FGISItemDropInfo& DropItemInfoIn)
{
	return true;
}

void UGISInventoryBaseComponent::ClientHideLootingWidget_Implementation()
{
	if (LootWidget)
		LootWidget->SetVisibility(ESlateVisibility::Hidden);
}
void UGISInventoryBaseComponent::PostInventoryInitialized()
{

}

void UGISInventoryBaseComponent::OnRep_TabUpdated()
{
	OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
}
void UGISInventoryBaseComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, Tabs, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, SlotUpdateInfo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, SlotSwapInfo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, CurrentPickupActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, LootFromPickup, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, TabUpdateInfo, COND_OwnerOnly);
}

bool UGISInventoryBaseComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	/*
		Should optimize it to replicate only items in array which changed.
		Don't know how though. Yet.
	*/

	for (const FGISTabInfo& TabInfo : Tabs.InventoryTabs)
	{
		for (const FGISSlotInfo& SlotItem : TabInfo.TabSlots)
		{
			if (SlotItem.ItemData)
			{
				WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotItem.ItemData), *Bunch, *RepFlags);
			}
		}
	}

	if (SlotUpdateInfo.SlotData)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotUpdateInfo.SlotData), *Bunch, *RepFlags);
	}
	if (SlotSwapInfo.LastSlotData)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotSwapInfo.LastSlotData), *Bunch, *RepFlags);
	}
	if (SlotSwapInfo.TargetSlotData)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotSwapInfo.TargetSlotData), *Bunch, *RepFlags);
	}
	for (const FGISLootSlotInfo& data : LootFromPickup.Loot)
	{
		if (data.SlotData)
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(data.SlotData), *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}
void UGISInventoryBaseComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	for (const FGISTabInfo& TabInfo : Tabs.InventoryTabs)
	{
		for (const FGISSlotInfo& SlotItem : TabInfo.TabSlots)
		{
			if (SlotItem.ItemData)
			{
				Objs.Add(const_cast<UGISItemData*>(SlotItem.ItemData));
			}
		}
	}
}

void UGISInventoryBaseComponent::InitializeInventoryTabs()
{
	int8 counter = 0;

	for (const FGISInventoryTabConfig& tabConf : InventoryConfig.TabConfigs)
	{
		FGISTabInfo TabInfo;
		TabInfo.NumberOfSlots = tabConf.NumberOfSlots;
		TabInfo.TabName = tabConf.TabName;
		TabInfo.Tags = tabConf.Tags;
		TabInfo.TabIndex = counter;
		TabInfo.bIsTabActive = tabConf.bIsTabActive;
		TabInfo.ItemCount = 0;
		for (int32 Index = 0; Index < tabConf.NumberOfSlots; Index++)
		{
			FGISSlotInfo SlotInfo;
			SlotInfo.SlotIndex = Index;
			SlotInfo.SlotTabIndex = counter;
			SlotInfo.CurrentInventoryComponent = this;
			SlotInfo.ItemData = nullptr;
			TabInfo.TabSlots.Add(SlotInfo);
		}
		Tabs.InventoryTabs.Add(TabInfo);
		counter++;
	}
}
void  UGISInventoryBaseComponent::InputSlotPressed(int32 TabIndex, int32 SlotIndex)
{
	//TODO:: Add valid index check...
	if (Tabs.GetItemData(TabIndex, SlotIndex))
	{
		Tabs.GetItemData(TabIndex, SlotIndex)->InputPressed();
	}
}
void UGISInventoryBaseComponent::InputSlotReleased(int32 TabIndex, int32 SlotIndex)
{
	if (Tabs.GetItemData(TabIndex, SlotIndex))
	{
		Tabs.GetItemData(TabIndex, SlotIndex)->InputReleased();
	}
}
void UGISInventoryBaseComponent::SwapTabItems(int32 OriginalTab, int32 TargetTab)
{
	int32 OrignalItemCount = Tabs.InventoryTabs[OriginalTab].TabSlots.Num();
	int32 TargetItemCount = Tabs.InventoryTabs[TargetTab].TabSlots.Num();
	//check which item count is bigger
	//to avoid copying into non existend array elements.
	//we always count against smaller.
	if (OrignalItemCount > TargetItemCount)
	{
		for (int32 Index = 0; Index < TargetItemCount; Index++)
		{
			UGISItemData* tempTargetData = GetItemDataInSlot(TargetTab, Index);
			SetItemDataInSlot(TargetTab, Index, GetItemDataInSlot(OriginalTab, Index));
			SetItemDataInSlot(OriginalTab,Index,tempTargetData);
		}
	}
	else
	{
		for (int32 Index = 0; Index < OrignalItemCount; Index++)
		{
			UGISItemData* tempTargetData = GetItemDataInSlot(TargetTab, Index);
			SetItemDataInSlot(TargetTab, Index, GetItemDataInSlot(OriginalTab, Index));
			SetItemDataInSlot(OriginalTab, Index, tempTargetData);
		}
	}
	/*
		OnRep_ReconstructTabs ? For UI.
		We don't need or want to reconstruct everything excepy updated tabs, or maybe even just slots
		on these tabs.

		Eiteher way I have to send index of tab to update.
	*/
}

void UGISInventoryBaseComponent::CopyItemsToTab(int32 OriginalTab, int32 TargetTab)
{
	int32 OrignalItemCount = GetSlotsInTab(OriginalTab);
	int32 TargetItemCount = GetSlotsInTab(TargetTab);
	//check which item count is bigger
	//to avoid copying into non existend array elements.
	//we always count against smaller.

	if (OrignalItemCount > TargetItemCount)
	{
		for (int32 Index = 0; Index < TargetItemCount; Index++)
		{
			CopyItemsFromTab(TargetTab, Index, OriginalTab, Index);
		}
	}
	else
	{
		for (int32 Index = 0; Index < OrignalItemCount; Index++)
		{
			CopyItemsFromTab(TargetTab, Index, OriginalTab, Index);
		}
	}
	TabUpdateInfo.ReplicationCounter++;
	TabUpdateInfo.TargetTabIndex = TargetTab;
	//I will need to call it on client. But how do I determine
	//changed tab index on client after tab is replicated ?
	//replicate index separatetly ?
	if (GetNetMode() == ENetMode::NM_Standalone)
		OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);

}

void UGISInventoryBaseComponent::CopyItemsFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 TargetTabIndex)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerCopyItemsFromOtherInventoryTab(OtherIn, TargetTabIndex);
	}
	else
	{
		if (OtherIn)
		{
			OtherIn->SetLastTargetTab(TargetTabIndex);
			OtherIn->SetLastOtherOriginInventory(this);
			LastTargetTab = TargetTabIndex;
			LastOtherOriginInventory = OtherIn;
			int32 OtherTabCount = OtherIn->Tabs.InventoryTabs.Num();

			for (int32 TabIndex = 0; TabIndex < OtherTabCount; TabIndex++)
			{
				if (LastOtherOriginTab != TabIndex)
				{
					OtherIn->SetLastOtherOriginTab(TabIndex);
					LastOtherOriginTab = TabIndex;
					int32 TargetItemCount = GetSlotsInTab(TargetTabIndex);
					int32 OtherItemCount = Tabs.InventoryTabs[TabIndex].TabSlots.Num();
					//check which item count is bigger
					//to avoid copying into non existend array elements.
					//we always count against smaller.

					if (OtherItemCount > TargetItemCount)
					{
						for (int32 Index = 0; Index < TargetItemCount; Index++)
						{
							SetItemDataInSlot(TargetTabIndex, Index, OtherIn->GetItemDataInSlot(TabIndex, Index));
							OnCopyItemsFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
						}
					}
					else
					{
						for (int32 Index = 0; Index < OtherItemCount; Index++)
						{
							SetItemDataInSlot(TargetTabIndex, Index, OtherIn->GetItemDataInSlot(TabIndex, Index));
							OnCopyItemsFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
						}
					}
					TabUpdateInfo.ReplicationCounter++;
					TabUpdateInfo.TargetTabIndex = TargetTabIndex;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
					return;
				}
			}
		}
	}
}

void UGISInventoryBaseComponent::CopyItemsToOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex)
{
	if (OtherIn)
	{
		int32 OtherSlotCount = OtherIn->GetSlotsInTab(OtherTabIndex);
		int32 TargetSlotCount = GetSlotsInTab(TargetTabIndex);
		if (OtherSlotCount > TargetSlotCount)
		{
			for (int32 Index = 0; Index < TargetSlotCount; Index++)
			{
				OtherIn->SetItemDataInSlot(OtherTabIndex, Index, GetItemDataInSlot(TargetTabIndex, Index));
				OnCopyItemsToOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
			}
		}
		else
		{
			for (int32 Index = 0; Index < OtherSlotCount; Index++)
			{
				OtherIn->SetItemDataInSlot(OtherTabIndex, Index, GetItemDataInSlot(TargetTabIndex, Index));
				OnCopyItemsToOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
			}
		}
		OtherIn->TabUpdateInfo.ReplicationCounter++;
		OtherIn->TabUpdateInfo.TargetTabIndex = TargetTabIndex;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OtherIn->OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
}

void UGISInventoryBaseComponent::CopyItemsFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex)
{
	if (OtherIn)
	{
		int32 OtherSlotCount = OtherIn->GetSlotsInTab(OtherTabIndex);
		int32 TargetSlotCount = GetSlotsInTab(TargetTabIndex);
		if (OtherSlotCount > TargetSlotCount)
		{
			for (int32 Index = 0; Index < TargetSlotCount; Index++)
			{
				SetItemDataInSlot(TargetTabIndex, Index, OtherIn->GetItemDataInSlot(OtherTabIndex,Index));
				OnCopyItemsFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
			}
		}
		else
		{
			for (int32 Index = 0; Index < OtherSlotCount; Index++)
			{
				SetItemDataInSlot(TargetTabIndex, Index, OtherIn->GetItemDataInSlot(OtherTabIndex, Index));
				OnCopyItemsFromOtherInventoryTab(Tabs.InventoryTabs[TargetTabIndex].TabSlots[Index].ItemData);
			}
		}
		TabUpdateInfo.ReplicationCounter++;
		TabUpdateInfo.TargetTabIndex = TargetTabIndex;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnTabChanged.ExecuteIfBound(TabUpdateInfo.TargetTabIndex);
	}
}

void UGISInventoryBaseComponent::ServerCopyItemsFromOtherInventoryTab_Implementation(class UGISInventoryBaseComponent* OtherIn, int32 TargetTabIndex)
{
	CopyItemsFromOtherInventoryTab(OtherIn, TargetTabIndex);
}

bool UGISInventoryBaseComponent::ServerCopyItemsFromOtherInventoryTab_Validate(class UGISInventoryBaseComponent* OtherIn, int32 TargetTabIndex)
{
	return true;
}
void UGISInventoryBaseComponent::OnCopyItemsFromOtherInventoryTab(class UGISItemData* DataIn)
{

}
void UGISInventoryBaseComponent::OnCopyItemsToOtherInventoryTab(class UGISItemData* DataIn)
{

}
int32 UGISInventoryBaseComponent::CountActiveTabs()
{
	int32 ActiveTabsCount = 0;
	for (const FGISTabInfo& tab : Tabs.InventoryTabs)
	{
		if (tab.bIsTabActive)
			ActiveTabsCount++;
	}

	return ActiveTabsCount;
}

FGISTabInfo& UGISInventoryBaseComponent::BP_GetOneTab(int32 TabIndexIn)
{
	return Tabs.InventoryTabs[TabIndexIn];
}
void UGISInventoryBaseComponent::BP_SetTabactive(int32 TabIndexIn, bool bIsTabActiveIn)
{
	Tabs.InventoryTabs[TabIndexIn].bIsTabActive = bIsTabActiveIn;
}

void UGISInventoryBaseComponent::BP_SetNextTab(int32 TabIndexIn)
{

}

FGISTabInfo& UGISInventoryBaseComponent::GetTabByName(FName TabNameIn)
{
	for (FGISTabInfo& tab : Tabs.InventoryTabs)
	{
		if (tab.TabName == TabNameIn)
		{
			return tab;
		}
	}
	return Tabs.InventoryTabs[0];
}

FGISTabInfo& UGISInventoryBaseComponent::BP_GetTabByName(FName TabNameIn)
{
	return GetTabByName(TabNameIn);
}