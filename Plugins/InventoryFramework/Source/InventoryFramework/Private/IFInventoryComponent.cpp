// Fill out your copyright notice in the Description page of Project Settings.

#include "IFInventoryComponent.h"

#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"

#include "IFItemBase.h"
#include "IFItemActorBase.h"
#include "IFInventoryInterface.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

void FIFItemData::SetOnItemChanged(FIFOnItemChangedEvent& Event)
{
	OnItemChanged = Event;
}
void FIFItemData::PreReplicatedRemove(const struct FIFItemContainer& InArraySerializer)
{
	if(Item)
		Item->OnItemRemoved(LocalIndex);
}

void FIFItemData::PostReplicatedAdd(const struct FIFItemContainer& InArraySerializer)
{
	int32 Idx = InArraySerializer.Items.IndexOfByKey(*this);

	LocalIndex = static_cast<uint8>(Idx);
	const_cast<FIFItemContainer&>(InArraySerializer).NetToLocal.Add(NetIndex, LocalIndex);
	const_cast<FIFItemContainer&>(InArraySerializer).LocalToNet.Add(LocalIndex, NetIndex);
	if (InArraySerializer.IC.IsValid() && InArraySerializer.IC->MaxSlots == (NetIndex+1))
	{
		InArraySerializer.IC->OnInventoryReady.Broadcast();
		if (IIFInventoryInterface* Interface = Cast<IIFInventoryInterface>(InArraySerializer.IC->GetOwner()))
		{
			Interface->OnInventoryReplicated(InArraySerializer.IC.Get());
		}
	}
	if (Item)
	{
		Item->OnItemAdded(LocalIndex);
		InArraySerializer.IC->OnItemAdded(Item, LocalIndex);
	}
}

void FIFItemData::PostReplicatedChange(const struct FIFItemContainer& InArraySerializer)
{
	InArraySerializer.IC->OnItemChangedEvent.Broadcast(NetIndex, LocalIndex);
	OnItemChanged.ExecuteIfBound(NetIndex, LocalIndex);
	if (Item)
	{
		switch (ChangeType)
		{
		case EIFChangeType::Added:
			Item->OnItemAdded(LocalIndex);
			InArraySerializer.IC->OnItemAdded(Item, LocalIndex);
			break;
		case EIFChangeType::Updated:
			Item->OnItemChanged(LocalIndex);
			break;
		case EIFChangeType::Removed:
			Item->OnItemRemoved(LocalIndex);
			break;
		default:
			break;
		}
		
	}
}
void FIFItemContainer::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{

}
void FIFItemContainer::AddItem(uint8 InNetIndex)
{
	uint8 LocalIndex = NetToLocal.FindRef(InNetIndex);
	FIFItemData& Item = Items[LocalIndex];


}
void FIFItemContainer::AddItem(class UIFItemBase* InItem, uint8 InNetIndex)
{
	uint8 LocalIndex = NetToLocal.FindRef(InNetIndex);
	FIFItemData& Item = Items[LocalIndex];
	Item.Item = InItem;
	Item.ChangeType = EIFChangeType::Added;
	Item.Item->OnItemAdded(LocalIndex);
	IC->OnItemAdded(Item.Item, Item.LocalIndex);

	MarkItemDirty(Item);
}
void FIFItemContainer::AddItemToFreeSlot(class UIFItemBase* InItem)
{
	for (FIFItemData& Item : Items)
	{
		if (!Item.Item)
		{
			Item.Item = InItem;
			//should be safe to call. On server it probabaly won't do anything and on standalone/clients will update widget.
			Item.OnSlotChanged();
			Item.Item->OnItemAdded(Item.LocalIndex);
			IC->OnItemAdded(Item.Item, Item.LocalIndex);
			Item.ChangeType = EIFChangeType::Added;
			MarkItemDirty(Item);
			break;
		}
	}
}
void FIFItemContainer::MoveItem(uint8 NewPosition, uint8 OldPosition)
{
	uint8 NewLocal = NetToLocal[NewPosition];
	uint8 OldLocal = NetToLocal[OldPosition];

	FIFItemData& NewItem = Items[NewLocal];
	FIFItemData& OldItem = Items[OldLocal];
	if (!OldItem.Item)
	{
		return;
	}
	UIFItemBase* NewSlotBackup = nullptr;
	
	if (NewItem.Item)
	{
		NewSlotBackup = DuplicateObject<UIFItemBase>(NewItem.Item, IC.Get());
		NewItem.Item->MarkPendingKill();
		Items[NewLocal].Item = nullptr;
	}

	NewItem.Item = DuplicateObject<UIFItemBase>(OldItem.Item, IC.Get());
	OldItem.Item->MarkPendingKill();
	OldItem.Item = nullptr;
	if (NewSlotBackup)
	{
		OldItem.Item = NewSlotBackup; //duplicate ?
	}
	if (NewItem.Item)
	{
		NewItem.Item->OnItemChanged(NewItem.LocalIndex);
		NewItem.ChangeType = EIFChangeType::Updated;
	}
	if (OldItem.Item)
	{
		OldItem.Item->OnItemChanged(OldItem.LocalIndex);
		OldItem.ChangeType = EIFChangeType::Updated;
	}

	MarkItemDirty(NewItem);
	MarkItemDirty(OldItem);

}
void FIFItemContainer::AddFromOtherInventory(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InNetIndex)
{
	uint8 SourceLocalIdx = Source->Inventory.NetToLocal[SourceNetIndex];
	uint8 LocalIdx = NetToLocal[InNetIndex];

	FIFItemData& SourceItem = const_cast<FIFItemData&>(Source->GetSlot(SourceLocalIdx));
	FIFItemData& LocalItem = Items[LocalIdx];

	LocalItem.Item = DuplicateObject<UIFItemBase>(SourceItem.Item, IC.Get());
	LocalItem.Item->OnItemAdded(LocalItem.LocalIndex);
	LocalItem.ChangeType = EIFChangeType::Added;
	LocalItem.Counter++;

	IC->OnItemAdded(LocalItem.Item, LocalItem.LocalIndex);
	LocalItem.OnSlotChanged();
	SourceItem.Item->MarkPendingKill();
	SourceItem.Item = nullptr;
	SourceItem.OnSlotChanged();

	MarkItemDirty(LocalItem);
	Source->Inventory.MarkItemDirty(SourceItem);
}

// Sets default values for this component's properties
UIFInventoryComponent::UIFInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bAutoRegister = true;

	MaxSlots = 8;
	AvailableSlots = 8;
	// ...
}

void UIFInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Inventory.IC = this;
}

// Called when the game starts
void UIFInventoryComponent::BeginPlay()
{
	Inventory.IC = this;
	Super::BeginPlay();
	

	/*
		Further steps
		2. Load Properties from external data source (JSON);
	*/
	// ...
	
}

// Called every frame
void UIFInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UIFInventoryComponent::InitializeInventory()
{
	ENetMode NM = GetOwner()->GetNetMode();
	ENetRole NR = GetOwnerRole();

	//Preallocate inventory items. We are not going to add/remove struct items
	//but we are going to modify their internals later.
	if ((NM == ENetMode::NM_DedicatedServer)
		|| (NM == ENetMode::NM_ListenServer)
		|| (NM == ENetMode::NM_Standalone))
	{
		uint8 AvailableCounter = 0;
		for (uint8 Idx = 0; Idx < MaxSlots; Idx++)
		{
			FIFItemData NewItem(Idx, Idx);
			if (AvailableCounter < AvailableSlots)
			{
				NewItem.bAvailable = true;
			}
			AvailableCounter++;
			Inventory.AddData(NewItem);
		}
		OnInventoryReady.Broadcast();
	}
	if (NM == ENetMode::NM_Standalone)
	{
		if (IIFInventoryInterface* Interface = Cast<IIFInventoryInterface>(GetOwner()))
		{
			Interface->OnInventoryReplicated(this);
		}
	}
}

void UIFInventoryComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME_CONDITION(UIFInventoryComponent, Inventory, COND_OwnerOnly);
}

void UIFInventoryComponent::MoveItemInInventory(uint8 NewLocalPostion, uint8 OldLocalPositin)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		uint8 NewNetPosition = Inventory.GetNetIndex(NewLocalPostion);
		uint8 OldNetPosition = Inventory.GetNetIndex(OldLocalPositin);

		ServerMoveItemInInventory(NewNetPosition, OldNetPosition);
		return;
	}
	uint8 NewNetPosition = Inventory.GetNetIndex(NewLocalPostion);
	uint8 OldNetPosition = Inventory.GetNetIndex(OldLocalPositin);
	Inventory.MoveItem(NewNetPosition, OldNetPosition);

	const FIFItemData& NewSlot = GetSlot(NewLocalPostion);
	const FIFItemData& OldSlot = GetSlot(OldLocalPositin);

	NewSlot.OnSlotChanged();
	OldSlot.OnSlotChanged();
}

void UIFInventoryComponent::ServerMoveItemInInventory_Implementation(uint8 NewNetPostion, uint8 OldNetPositin)
{
	Inventory.MoveItem(NewNetPostion, OldNetPositin);
}
bool UIFInventoryComponent::ServerMoveItemInInventory_Validate(uint8 NewNetPostion, uint8 OldNetPositin)
{
	return true;
}

void UIFInventoryComponent::AddAllItemsFromActor(class AIFItemActorBase* Source)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddAllItemsFromActor(Source);
		return;
	}
	TArray<TSoftClassPtr<UIFItemBase>> Items = Source->GetAllItems();
	for (const TSoftClassPtr<UIFItemBase> Item : Items)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoadedFreeSlot, Item);

		Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
	}
}
void UIFInventoryComponent::ServerAddAllItemsFromActor_Implementation(class AIFItemActorBase* Source)
{
	TArray<TSoftClassPtr<UIFItemBase>> Items = Source->GetAllItems();
	for (const TSoftClassPtr<UIFItemBase> Item : Items)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoadedFreeSlot, Item);

		Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
	}
}
bool UIFInventoryComponent::ServerAddAllItemsFromActor_Validate(class AIFItemActorBase* Sourcex)
{
	return true;
}


void UIFInventoryComponent::AddItemFromOtherInventory(class UIFInventoryComponent* Source
	, uint8 SourceLocalIndex
	, uint8 InLocalIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		uint8 SourceNetIndex = Source->Inventory.GetNetIndex(SourceLocalIndex);
		uint8 NetIndex = Inventory.GetNetIndex(InLocalIndex);
		ServerAddItemFromOtherInventory(Source, SourceNetIndex, NetIndex);
		return;
	}
	Inventory.AddFromOtherInventory(Source, SourceLocalIndex, InLocalIndex);
}
void UIFInventoryComponent::ServerAddItemFromOtherInventory_Implementation(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InNetIndex)
{
	Inventory.AddFromOtherInventory(Source, SourceNetIndex, InNetIndex);
}
bool UIFInventoryComponent::ServerAddItemFromOtherInventory_Validate(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InNetIndex)
{
	return true;
}
void UIFInventoryComponent::AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		uint8 NetIndex = Inventory.GetNetIndex(InLocalIndex);
		ServerAddItemFromClass(Item.ToSoftObjectPath(), NetIndex);
		return;
	}
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	uint8 NetIndex = Inventory.GetNetIndex(InLocalIndex);
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoaded, Item, NetIndex);
	Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
}
void UIFInventoryComponent::BP_AddAllItemsFromActor(class AIFItemActorBase* Source)
{
	AddAllItemsFromActor(Source);
}


void UIFInventoryComponent::ServerAddItemFromClass_Implementation(FSoftObjectPath Item, uint8 InNetIndex)
{
	FStreamableManager& Manager =  UAssetManager::GetStreamableManager();
	TSoftClassPtr<UIFItemBase> It(Item);
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoaded, It, InNetIndex);

	UE_LOG(LogTemp, Warning, TEXT("ServerAddItemFromClass_Implementation %s"), *Item.ToString());
	Manager.RequestAsyncLoad(Item, Delegate);
}
bool UIFInventoryComponent::ServerAddItemFromClass_Validate(FSoftObjectPath Item, uint8 InNetIndex)
{
	return true;
}
void UIFInventoryComponent::BP_AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex)
{
	AddItemFromClass(Item, InLocalIndex);
}

void UIFInventoryComponent::RemoveItem(uint8 InLocalIndex)
{

}

void UIFInventoryComponent::OnItemLoadedFreeSlot(TSoftClassPtr<class UIFItemBase> InItem)
{
	TSubclassOf<UIFItemBase> ItemClass = InItem.Get();

	UIFItemBase* Item = NewObject<UIFItemBase>(this, ItemClass);
	Inventory.AddItemToFreeSlot(Item);

	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.Unload(InItem.ToSoftObjectPath());
}
void UIFInventoryComponent::OnItemLoaded(TSoftClassPtr<class UIFItemBase> InItem, uint8 InNetIndex)
{
	TSubclassOf<UIFItemBase> ItemClass = InItem.Get();

	UIFItemBase* Item = NewObject<UIFItemBase>(this, ItemClass);
	Inventory.AddItem(Item, InNetIndex);

	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.Unload(InItem.ToSoftObjectPath());
}
FSimpleMulticastDelegate& UIFInventoryComponent::GetOnInventoryRead()
{
	return OnInventoryReady;
}

FIFOnItemChanged& UIFInventoryComponent::GetItemChangedEvent()
{
	return OnItemChangedEvent;
}
bool UIFInventoryComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (const FIFItemData& Item : Inventory.Items)
	{
		if (Item.Item)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UIFItemBase*>(Item.Item), *Bunch, *RepFlags);
	}
	return WroteSomething;
}