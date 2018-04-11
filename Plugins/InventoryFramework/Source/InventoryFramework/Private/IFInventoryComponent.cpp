// Fill out your copyright notice in the Description page of Project Settings.

#include "IFInventoryComponent.h"

#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"

#include "IFItemBase.h"
#include "IFItemActorBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"


void FIFItemData::PreReplicatedRemove(const struct FIFItemContainer& InArraySerializer)
{

}

void FIFItemData::PostReplicatedAdd(const struct FIFItemContainer& InArraySerializer)
{
	int32 Idx = InArraySerializer.Items.IndexOfByKey(*this);

	LocalIndex = static_cast<uint8>(Idx);
	const_cast<FIFItemContainer&>(InArraySerializer).NetToLocal.Add(NetIndex, LocalIndex);
	const_cast<FIFItemContainer&>(InArraySerializer).LocalToNet.Add(LocalIndex, NetIndex);
}

void FIFItemData::PostReplicatedChange(const struct FIFItemContainer& InArraySerializer)
{
	int asd = 0;
	if (asd)
	{

	}
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
	MarkItemDirty(Item);
}
void FIFItemContainer::AddItemToFreeSlot(class UIFItemBase* InItem)
{
	for (FIFItemData& Item : Items)
	{
		if (!Item.Item)
		{
			Item.Item = InItem;
			MarkItemDirty(Item);
			break;
		}
	}
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
	// ...
}


// Called when the game starts
void UIFInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Inventory.IC = this;

	ENetMode NM = GetOwner()->GetNetMode();
	ENetRole NR = GetOwnerRole();

	//Preallocate inventory items. We are not going to add/remove struct items
	//but we are going to modify their internals later.
	if ((NM == ENetMode::NM_DedicatedServer)
		|| (NM == ENetMode::NM_ListenServer)
		|| (NM == ENetMode::NM_Standalone))
	{
		for (int32 Idx = 0; Idx < 8; Idx++)
		{
			FIFItemData NewItem(Idx, Idx);

			Inventory.AddData(NewItem);
		}
	}
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

void UIFInventoryComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME_CONDITION(UIFInventoryComponent, Inventory, COND_OwnerOnly);
}

void UIFInventoryComponent::AddItemFromActor(class AIFItemActorBase* Source
	, uint8 SourceIndex
	, uint8 InLocalIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		uint8 NetIndex = Inventory.GetNetIndex(InLocalIndex);
		ServerAddItemFromActor(Source, SourceIndex, NetIndex);
		return;
	}
	Inventory.AddItem(InLocalIndex);

}
void UIFInventoryComponent::ServerAddItemFromActor_Implementation(class AIFItemActorBase* Source
	, uint8 SourceIndex
	, uint8 InNetIndex)
{
	TArray<TSoftClassPtr<UIFItemBase>> Items = Source->GetAllItems();
	for (const TSoftClassPtr<UIFItemBase> Item : Items)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoadedFreeSlot, Item);

		Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
	}
	
}
bool UIFInventoryComponent::ServerAddItemFromActor_Validate(class AIFItemActorBase* Source
	, uint8 SourceIndex
	, uint8 InNetIndex)
{
	return true;
}


void UIFInventoryComponent::AddItemFromOtherInventory(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InLocalIndex)
{

}
void UIFInventoryComponent::ServerAddItemFromOtherInventory_Implementation(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InLocalIndex)
{

}
bool UIFInventoryComponent::ServerAddItemFromOtherInventory_Validate(class UIFInventoryComponent* Source
	, uint8 SourceNetIndex
	, uint8 InLocalIndex)
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
	AddItemFromActor(Source, 0, 0);
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