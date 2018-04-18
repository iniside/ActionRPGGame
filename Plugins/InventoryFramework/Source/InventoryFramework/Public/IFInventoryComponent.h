// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IFInventoryComponent.generated.h"

//NetIndex, LocalIndex
DECLARE_MULTICAST_DELEGATE_ThreeParams(FIFItemEvent, uint8, uint8, class UIFItemBase*);
DECLARE_MULTICAST_DELEGATE(FIFOnInventoryChanged);

UENUM()
enum class EIFChangeType : uint8
{
	Added,
	Updated,
	Removed
};

USTRUCT(BlueprintType)
struct INVENTORYFRAMEWORK_API FIFItemData : public FFastArraySerializerItem
{
	GENERATED_BODY()
		friend class UIFInventoryComponent;
	friend struct FIFItemContainer;

protected:
	UPROPERTY()
		class UIFItemBase* Item;

	/*
		Indexes below are used to resolve item mapping between server and client,
		because there is no guarnteed order of replication.
	*/
	/*
		Index in Array of this item on Server.
	*/
	UPROPERTY(BlueprintReadOnly)
		uint8 NetIndex;

	/*
		LIndex in Array of this item on Client.
	*/
	UPROPERTY(NotReplicated, BlueprintReadOnly)
		uint8 LocalIndex;

	/* Is slot currently available */
	UPROPERTY(BlueprintReadOnly)
		uint8 bAvailable:1;

	/* 
		Do Not set Item to nullptr instead set this item to dirty, to indicate that items has been "removed", so we have a Chance
		to call OnItemRemoved() on item on clients.
	*/
	UPROPERTY(BlueprintReadOnly)
		uint8 bDirty : 1;

	UPROPERTY()
		EIFChangeType ChangeType;

	UPROPERTY()
		uint8 Counter;
public:

	FIFItemData()
		: Item(nullptr)
		, NetIndex(INDEX_NONE)
		, LocalIndex(INDEX_NONE)
		, bAvailable(false)
		, Counter(0)
	{}

	FIFItemData(uint8 InNetIndex, uint8 InLocalIndex)
		: Item(nullptr)
		, NetIndex(InNetIndex)
		, LocalIndex(InLocalIndex)
		, bAvailable(false)
		, Counter(0)
	{}
	inline uint8 GetNetIndex() const
	{
		return NetIndex;
	}
	inline uint8 GetLocalIndex() const
	{
		return LocalIndex;
	}

	/**
	* Called right before deleting element during replication.
	*
	* @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	*
	* NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	*/
	void PreReplicatedRemove(const struct FIFItemContainer& InArraySerializer);
	/**
	* Called after adding and serializing a new element
	*
	* @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	*
	* NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	*/
	void PostReplicatedAdd(const struct FIFItemContainer& InArraySerializer);
	/**
	* Called after updating an existing element with new data
	*
	* @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	* NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	*/
	void PostReplicatedChange(const struct FIFItemContainer& InArraySerializer);

	bool operator==(const FIFItemData& Right) const
	{
		return NetIndex == Right.NetIndex;
	}
};

USTRUCT()
struct INVENTORYFRAMEWORK_API FIFItemContainer : public FFastArraySerializer
{	
	GENERATED_BODY()
		friend class UIFInventoryComponent;
	UPROPERTY()
		TArray<FIFItemData> Items;

	TWeakObjectPtr<class UIFInventoryComponent> IC;
	/* NetIndex, LocalIndex */
	TMap<uint8, uint8> NetToLocal;
	TMap<uint8, uint8> LocalToNet;

protected:
	void AddData(const FIFItemData& InItem)
	{
		
		int32 Idx = Items.Add(InItem);
		MarkItemDirty(Items[Idx]);
		MarkArrayDirty();
		NetToLocal.Add(InItem.NetIndex, InItem.LocalIndex);
		LocalToNet.Add(InItem.LocalIndex, InItem.NetIndex);
	}
	uint8 GetNetIndex(uint8 LocalIndex)
	{
		return LocalToNet.FindRef(LocalIndex);
	}
	void AddItem(uint8 InNetIndex);
	void AddItem(class UIFItemBase* InItem, uint8 InNetIndex);
	void AddItemToFreeSlot(class UIFItemBase* InItem);
	void MoveItem(uint8 NewPosition, uint8 OldPosition);

	void AddFromOtherInventory(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex
		, uint8 InNetIndex);

	void AddFromOtherInventoryAny(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex);

	TArray<uint8> GetLocalItemIdxs(TSubclassOf<UIFItemBase> ItemClass);

	void RemoveItem(uint8 InNetIndex);

	template<typename T>
	TArray<T*> GetItems(TSubclassOf<T> ItemClass)
	{
		TArray<T*> Indexes;
		for (uint8 Idx = 0; Idx < Items.Num(); Idx++)
		{
			if (Items[Idx].Item && Items[Idx].Item->IsA(ItemClass))
			{
				Indexes.Add(Cast<T>(Items[Idx].Item));
			}
		}
		return Indexes;
	}

public:
	inline TArray<FIFItemData>& GetItems() { return Items; }


	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FIFItemData, FIFItemContainer>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FIFItemContainer > : public TStructOpsTypeTraitsBase2<FIFItemContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FIFSlotAcceptedClasses
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UIFItemBase>> AcceptedClasses;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYFRAMEWORK_API UIFInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	friend struct FIFItemContainer;
	friend struct FIFItemData;
protected:
	UPROPERTY(Replicated)
		FIFItemContainer Inventory;

	FIFItemEvent OnItemAddedEvent;
	FIFItemEvent OnItemUpdatedEvent;
	FIFItemEvent OnItemRemovedEvent;
	FIFOnInventoryChanged OnInventoryChanged;

	/*
		Which items this inventory accept.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TArray<TSubclassOf<class UIFItemBase>> AcceptedClasses;

	/*
		Which items slot will accept.

		Order is Equvalent to slot order on server.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TArray<FIFSlotAcceptedClasses> AcceptedSlotClasses;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		uint8 MaxSlots;

	/*
		Currently available slots (must be smaller or equal to max slots);
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		uint8 AvailableSlots;

	/* Called when all alots are created */
	FSimpleMulticastDelegate OnInventoryReady;

public:	
	// Sets default values for this component's properties
	UIFInventoryComponent();

protected:
	virtual void InitializeComponent() override;;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeInventory();

	inline FIFItemContainer& GetInventory() { return Inventory; }

	inline FIFItemEvent& GetOnItemAdded() { return OnItemAddedEvent; }
	inline FIFItemEvent& GetOnItemUpdated() { return OnItemUpdatedEvent; }
	inline FIFItemEvent& GetOnItemRemoved() { return OnItemRemovedEvent; }

	inline uint8 GetMaxSlots()
	{
		return MaxSlots;
	}

	inline const FIFItemData& GetSlot(uint8 Idx)
	{
		return Inventory.Items[Idx];
	}

	inline UIFItemBase* GetItem(uint8 InLocalIndex)
	{
		return Inventory.Items[InLocalIndex].Item;
	}
	template<typename T>
	T* GetItem(uint8 InLocalIndex)
	{
		return Cast<T>(Inventory.Items[InLocalIndex].Item);
	}

	TArray<uint8> GetLocalItemIdxs(TSubclassOf<UIFItemBase> ItemClass)
	{
		return Inventory.GetLocalItemIdxs(ItemClass);
	}

	template<typename T>
	TArray<T*> GetItems(TSubclassOf<T> ItemClass)
	{
		return Inventory.GetItems<T>(ItemClass);
	}

	/* 
		Check if item can be droped/added to this inventory/slot.
		First check if inventory accepts, If it passes it will check slot.
	*/
	bool AcceptItem(UIFItemBase* Item, uint8 InLocaLIndex);
	/*
		Move item from old position to new position.
		If there was already item in new position it will be swapped with the moved item;
	*/
	void MoveItemInInventory(uint8 NewLocalPostion, uint8 OldLocalPositin);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMoveItemInInventory(uint8 NewNetPostion, uint8 OldNetPositin);
	void ServerMoveItemInInventory_Implementation(uint8 NewNetPostion, uint8 OldNetPositin);
	bool ServerMoveItemInInventory_Validate(uint8 NewNetPostion, uint8 OldNetPositin);
	/* 
		Adds new item at slot specified slot 
		Source - Droped item from which we will transfer item
		SourceIndex - Index
	*/
	void AddAllItemsFromActor(class AIFItemActorBase* Source);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddAllItemsFromActor(class AIFItemActorBase* Source);
	void ServerAddAllItemsFromActor_Implementation(class AIFItemActorBase* Source);
	bool ServerAddAllItemsFromActor_Validate(class AIFItemActorBase* Source);
	UFUNCTION(BlueprintCallable, Category = "InventoryFramework")
		void BP_AddAllItemsFromActor(class AIFItemActorBase* Source);


	/*
		Moves item to this inventory from the source Inventory. If item already exist it will be swapped (if possible).
	*/
	void AddItemFromOtherInventory(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex
		, uint8 InLocalIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromOtherInventory(class UIFInventoryComponent* Source
			, uint8 SourceNetIndex
			, uint8 InLocalIndex);
	void ServerAddItemFromOtherInventory_Implementation(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex
		, uint8 InNetIndex);
	bool ServerAddItemFromOtherInventory_Validate(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex
		, uint8 InNetIndex);


	void AddItemFromOtherInventoryAny(class UIFInventoryComponent* Source
		, uint8 SourceLocalIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromOtherInventoryAny(class UIFInventoryComponent* Source
			, uint8 SourceNetIndex);
	void ServerAddItemFromOtherInventoryAny_Implementation(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex);
	bool ServerAddItemFromOtherInventoryAny_Validate(class UIFInventoryComponent* Source
		, uint8 SourceNetIndex);

	/* Adds item from class. Realistically you should never call it on client. */
	void AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromClass(const FSoftObjectPath& Item, uint8 InLocalIndex);
	
	void ServerAddItemFromClass_Implementation(FSoftObjectPath Item, uint8 InNetIndex);
	bool ServerAddItemFromClass_Validate(FSoftObjectPath Item, uint8 InNetIndex);
	UFUNCTION(BlueprintCallable, Category = "InventoryFramework")
		void BP_AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex);

	virtual void OnItemAdded(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnItemChanged(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnItemRemoved(uint8 LocalIndex) {};
	void RemoveItem(uint8 InLocalIndex);



	UFUNCTION()
		void OnItemLoadedFreeSlot(TSoftClassPtr<class UIFItemBase> InItem);

	UFUNCTION()
		void OnItemLoaded(TSoftClassPtr<class UIFItemBase> InItem, uint8 InNetIndex);

	FSimpleMulticastDelegate& GetOnInventoryRead();
	
	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
};
