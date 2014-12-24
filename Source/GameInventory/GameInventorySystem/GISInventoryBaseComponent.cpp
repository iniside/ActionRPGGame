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
	bRemoveItemsFromInvetoryOnDrag = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.bRunOnAnyThread = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bAllowConcurrentTick = true;
	OnItemLooted.AddDynamic(this, &UGISInventoryBaseComponent::ConstructLootPickingWidget);
	InventoryVisibility = ESlateVisibility::Hidden;
	LootWindowVisibility = ESlateVisibility::Hidden;
}


void UGISInventoryBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();
	//if (!bHasBeenInitialized)
	//{
		ENetRole CurrentRole = GetOwnerRole();
		ENetMode CurrentNetMode = GetNetMode();

		//PrimaryComponentTick.bStartWithTickEnabled = bTickInventory;
		if (CurrentRole == ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
			InitializeInventoryTabs();

		if (CurrentRole < ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
		{
			if (InventoryContainerClass)
			{
				UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
				InventoryContainer = ConstructObject<UGISContainerBaseWidget>(InventoryContainerClass, Outer);
				if (InventoryContainer)
				{
					ULocalPlayer* Player = World->GetFirstLocalPlayerFromController(); //temporary
					InventoryContainer->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
					InventoryContainer->Initialize();
					InventoryContainer->InventoryComponent = this;
					InventoryContainer->SetVisibility(InventoryVisibility);
					//call last
					InventoryContainer->InitializeContainer();
				}
			}

			UObject* Outer = GetWorld()->GetGameInstance() ? StaticCast<UObject*>(GetWorld()->GetGameInstance()) : StaticCast<UObject*>(GetWorld());
			if (LootWidgetClass)
			{
				LootWidget = ConstructObject<UGISLootContainerBaseWidget>(LootWidgetClass, Outer);
				if (LootWidget)
				{
					ULocalPlayer* Player = GetWorld()->GetFirstLocalPlayerFromController(); //temporary
					LootWidget->SetPlayerContext(FLocalPlayerContext(Player)); //temporary
					LootWidget->Initialize();
					LootWidget->SetVisibility(LootWindowVisibility);
				}
			}
		}

		if (CurrentRole == ROLE_Authority || CurrentNetMode == ENetMode::NM_Standalone)
			ClientLoadInventory();
	//}
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
	//if (SlotUpdateInfo.SlotComponent.IsValid())
	//	SlotUpdateInfo.SlotData = SlotUpdateInfo.SlotComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].TabSlots[SlotUpdateInfo.SlotIndex].ItemData;
	//OnItemAdded.Broadcast(SlotUpdateInfo);
}

void UGISInventoryBaseComponent::OnRep_SlotUpdate()
{
//	if (SlotUpdateInfo.SlotComponent.IsValid())
//		SlotUpdateInfo.SlotData = SlotUpdateInfo.SlotComponent->Tabs.InventoryTabs[SlotUpdateInfo.TabIndex].TabSlots[SlotUpdateInfo.SlotIndex].ItemData;
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

FGISInventoryTab UGISInventoryBaseComponent::GetInventoryTabs()
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
					//Slot.ItemData->OnItemRemovedFromSlot();
					SlotUpdateInfo.TabIndex = TabInfo.TabIndex;
					SlotUpdateInfo.SlotIndex = Slot.SlotIndex;
					SlotUpdateInfo.SlotData = Slot.ItemData;
					SlotUpdateInfo.SlotComponent = this;
					if (GetNetMode() == ENetMode::NM_Standalone)
						OnItemAdded.Broadcast(SlotUpdateInfo);

					ClientUpdateInventory(SlotUpdateInfo);
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
	if (!TargetSlotType.CurrentInventoryComponent->RequiredTags.MatchesAny(LastSlotType.CurrentInventoryComponent->OwnedTags, false))
		return;
	
	//next check should be against item tags, but that's later!
	if (LastSlotType.CurrentInventoryComponent->bRemoveItemsFromInvetoryOnDrag)
	{

		//Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotTabIndex].ItemData
		if (TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData == nullptr)
		{

			TWeakObjectPtr<UGISItemData> TargetItem = LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData;
			TWeakObjectPtr<UGISItemData> LastItem = TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData; //Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData;

			TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData = TargetItem;
			LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData = nullptr;
			TargetItem->SetWorld(GetWorld());
			TargetItem->SetCurrentOwner(GetOwner());
			TargetItem->OnItemAddedToSlot();
			//FGISSlotSwapInfo SlotSwapInfo;

			SlotSwapInfo.LastSlotIndex = LastSlotType.SlotIndex;
			SlotSwapInfo.LastTabIndex = LastSlotType.SlotTabIndex;
			SlotSwapInfo.LastSlotData = LastItem;
			SlotSwapInfo.LastSlotComponent = LastSlotType.CurrentInventoryComponent;
			SlotSwapInfo.TargetSlotIndex = TargetSlotType.SlotIndex;
			SlotSwapInfo.TargetTabIndex = TargetSlotType.SlotTabIndex;
			SlotSwapInfo.TargetSlotData = TargetItem;
			SlotSwapInfo.TargetSlotComponent = TargetSlotType.CurrentInventoryComponent;
			if (GetNetMode() == ENetMode::NM_Standalone)
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			//ClientSlotSwap(SlotSwapInfo);
		}
		else
		{
			TWeakObjectPtr<UGISItemData> TargetItem = LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData;
			TWeakObjectPtr<UGISItemData> LastItem = TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData; //Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData;

			TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData = TargetItem;
			LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData = LastItem;

			TargetItem->SetWorld(GetWorld());
			TargetItem->SetCurrentOwner(GetOwner());
			LastItem->SetWorld(GetWorld());
			LastItem->SetCurrentOwner(GetOwner());
			TargetItem->OnItemAddedToSlot();
			LastItem->OnItemAddedToSlot();

			//FGISSlotSwapInfo SlotSwapInfo;
			SlotSwapInfo.LastSlotIndex = LastSlotType.SlotIndex;
			SlotSwapInfo.LastTabIndex = LastSlotType.SlotTabIndex;
			SlotSwapInfo.LastSlotData = LastItem;
			SlotSwapInfo.LastSlotComponent = LastSlotType.CurrentInventoryComponent;
			SlotSwapInfo.TargetSlotIndex = TargetSlotType.SlotIndex;
			SlotSwapInfo.TargetTabIndex = TargetSlotType.SlotTabIndex;
			SlotSwapInfo.TargetSlotData = TargetItem;
			SlotSwapInfo.TargetSlotComponent = TargetSlotType.CurrentInventoryComponent;
			if (GetNetMode() == ENetMode::NM_Standalone)
				OnItemSlotSwapped.Broadcast(SlotSwapInfo);
			//ClientSlotSwap(SlotSwapInfo);
		}
	}
	else
	{ //if there is item in target slot. Remove it. or override it.
		TWeakObjectPtr<UGISItemData> TargetItem = LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData;
		TWeakObjectPtr<UGISItemData> LastItem = TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData; //Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData;
		TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData = nullptr;
		TargetSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[TargetSlotType.SlotTabIndex].TabSlots[TargetSlotType.SlotIndex].ItemData = TargetItem;
		//LastSlotType.CurrentInventoryComponent->Tabs.InventoryTabs[LastSlotType.SlotTabIndex].TabSlots[LastSlotType.SlotIndex].ItemData = LastItem;

		TargetItem->SetWorld(GetWorld());
		TargetItem->SetCurrentOwner(GetOwner());
		//LastItem->SetWorld(GetWorld());
		TargetItem->OnItemAddedToSlot();
		//LastItem->OnItemAddedToSlot();

		//since we won't be removing anything from last inventory
		//we don't really need any information about it.
		//all we need to know if the target slot
		//contained any item. So we can remove widgets on client side.
		SlotSwapInfo.LastSlotIndex = TargetSlotType.SlotIndex;
		SlotSwapInfo.LastTabIndex = TargetSlotType.SlotTabIndex;
		SlotSwapInfo.LastSlotData = LastItem;
		SlotSwapInfo.LastSlotComponent = LastSlotType.CurrentInventoryComponent;
		SlotSwapInfo.TargetSlotIndex = TargetSlotType.SlotIndex;
		SlotSwapInfo.TargetTabIndex = TargetSlotType.SlotTabIndex;
		SlotSwapInfo.TargetSlotData = TargetItem;
		SlotSwapInfo.TargetSlotComponent = TargetSlotType.CurrentInventoryComponent;
		if (GetNetMode() == ENetMode::NM_Standalone)
			OnItemSlotSwapped.Broadcast(SlotSwapInfo);
		//ClientSlotSwap(SlotSwapInfo);
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

void UGISInventoryBaseComponent::RemoveItem(const FGISSlotInfo& TargetSlotType)
{

}

void UGISInventoryBaseComponent::GetLootContainer(class AGISPickupActor* LootContainer)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerGetLootContainer(LootContainer);
	}
	else
	{
		if (!LootContainer->bIsCurrentlyBeingLooted)
		{
			CurrentPickupActor = LootContainer;
			LootContainer->bIsCurrentlyBeingLooted = true;
			for (UGISItemData* Item : LootContainer->ItemToLoot)
			{
				UGISItemData* test = ConstructObject<UGISItemData>(Item->GetClass(), this, NAME_None, RF_NoFlags, Item);
				LootedItems.Add(test);
			}
			SetComponentTickEnabled(true);
			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				OnRep_LootedItems();
			}
			//ClientConstructWidget();
		}
	}
}
void UGISInventoryBaseComponent::OnRep_LootedItems()
{
	ConstructLootPickingWidget();
}
void UGISInventoryBaseComponent::OnRep_PickupActor()
{
	//ConstructLootPickingWidget();
}
void UGISInventoryBaseComponent::ClientConstructWidget_Implementation()
{
	ConstructLootPickingWidget();
}
void UGISInventoryBaseComponent::ConstructLootPickingWidget()
{
	if (CurrentPickupActor)
	{
		if (LootWidget)
		{
			CurrentPickupActor->InteractingInventory = this;
			int32 ItemCount = LootedItems.Num();
			TArray<FGISLootSlotInfo> LootSlotInfos;
			for (int32 Index = 0; Index < ItemCount; Index++)
			{
				FGISLootSlotInfo LootInfo;
				LootInfo.SlotIndex = Index;
				LootInfo.SlotData = LootedItems[Index];
			//	LootInfo.OwningPickupActor = CurrentPickupActor;
				LootInfo.SlotComponent = this;
				LootSlotInfos.Add(LootInfo);
			}
			LootWidget->ItemsInfos = LootSlotInfos;
			//LootWidget->OwningPickupActor = CurrentPickupActor;
			LootWidget->InitializeLootWidget();
			LootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		//LootContainer->InteractingInventory = this;
		//LootContainer->OpenLootWindow();
	}
}

void UGISInventoryBaseComponent::LootItems()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerLootItems();
	}
	else
	{
		if (!CurrentPickupActor)
			return;


		for (UGISItemData* Item : LootedItems)
		{
			AddItemToInventory(Item);
		}
	}
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
void UGISInventoryBaseComponent::LootOneItem(int32 ItemIndex)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		SeverLootOneItem(ItemIndex);
	}
	else
	{
		if (CurrentPickupActor)
		{
		//	UGISItemData* test = ConstructObject<UGISItemData>(CurrentPickupActor->ItemToLoot[ItemIndex]->GetClass(), this, NAME_None, RF_NoFlags, CurrentPickupActor->ItemToLoot[ItemIndex]);
			
			//if it is not valid index both arrays, then something is wrong.
			if (LootedItems.IsValidIndex(ItemIndex) && CurrentPickupActor->ItemToLoot.IsValidIndex(ItemIndex))
			{
				AddItemToInventory(LootedItems[ItemIndex]);
				//ok we removed one item. We need to rconstruct widgets, indexes etc, to make sure arry
				//have proper indexes in first place.
				LootedItems.RemoveAt(ItemIndex, 1, true);
				CurrentPickupActor->ItemToLoot.RemoveAt(ItemIndex, 1, true);
			}
			//reconstruct widget.
			if (GetNetMode() == ENetMode::NM_Standalone)
			{
				OnRep_LootedItems();
			}
		//	CurrentPickupActor->ItemToLoot = LootedItems;
		//	ClientReconstructLootWidget();
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


void UGISInventoryBaseComponent::ServerGetLootContainer_Implementation(class AGISPickupActor* LootContainer)
{
	GetLootContainer(LootContainer);
}
bool UGISInventoryBaseComponent::ServerGetLootContainer_Validate(class AGISPickupActor* LootContainer)
{
	return true;
}

void UGISInventoryBaseComponent::ClientReconstructLootWidget_Implementation()
{
	OnItemLooted.Broadcast();
}

void UGISInventoryBaseComponent::ClientUpdateInventory_Implementation(const FGISSlotUpdateData& SlotUpdateInfoIn)
{
	//OnItemAdded.Broadcast(SlotUpdateInfoIn);
}
void UGISInventoryBaseComponent::ClientSlotSwap_Implementation(const FGISSlotSwapInfo& SlotSwapInfoIn)
{
	OnItemSlotSwapped.Broadcast(SlotSwapInfoIn);
}

void UGISInventoryBaseComponent::ClientLoadInventory_Implementation()
{
	OnInventoryLoaded.Broadcast();
}
void UGISInventoryBaseComponent::ClientHideLootingWidget_Implementation()
{
	if (LootWidget)
		LootWidget->SetVisibility(ESlateVisibility::Hidden);
}
void UGISInventoryBaseComponent::PostInventoryInitialized()
{

}
FGISTabInfo UGISInventoryBaseComponent::BP_GetOneTab(int32 TabIndexIn)
{
	return Tabs.InventoryTabs[TabIndexIn];
}
void UGISInventoryBaseComponent::BP_SetTabactive(int32 TabIndexIn, bool bIsTabActiveIn)
{
	Tabs.InventoryTabs[TabIndexIn].bIsTabActive = bIsTabActiveIn;
}
void UGISInventoryBaseComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, Tabs, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, SlotUpdateInfo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, SlotSwapInfo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, CurrentPickupActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGISInventoryBaseComponent, LootedItems, COND_OwnerOnly);
}

bool UGISInventoryBaseComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const FGISTabInfo& TabInfo : Tabs.InventoryTabs)
	{
		for (const FGISSlotInfo& SlotItem : TabInfo.TabSlots)
		{
			if (SlotItem.ItemData.IsValid())
			{
				WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotItem.ItemData.Get()), *Bunch, *RepFlags);
			}
		}
	}

	if (SlotUpdateInfo.SlotData.IsValid())
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotUpdateInfo.SlotData.Get()), *Bunch, *RepFlags);
	}
	if (SlotSwapInfo.LastSlotData.IsValid())
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotSwapInfo.LastSlotData.Get()), *Bunch, *RepFlags);
	}
	if (SlotSwapInfo.TargetSlotData.IsValid())
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(SlotSwapInfo.TargetSlotData.Get()), *Bunch, *RepFlags);
	}
	for (const UGISItemData* data : LootedItems)
	{
		if (data)
		{
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGISItemData*>(data), *Bunch, *RepFlags);
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
			if (SlotItem.ItemData.IsValid())
			{
				Objs.Add(const_cast<UGISItemData*>(SlotItem.ItemData.Get()));
			}
		}
	}
}

void UGISInventoryBaseComponent::InitializeInventoryTabs()
{
	int8 counter = 0;
	for (const FGISSlotsInTab& SlotInfo : InitialTabInfo)
	{

		FGISTabInfo TabInfo;
		TabInfo.NumberOfSlots = SlotInfo.NumberOfSlots;
		TabInfo.TabIndex = counter;

		for (int32 Index = 0; Index < SlotInfo.NumberOfSlots; Index++)
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