// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "IFTypes.h"
#include "IFItemBase.h"
#include "IFInventoryComponent.generated.h"

//NetIndex, LocalIndex

DECLARE_LOG_CATEGORY_EXTERN(IFLog, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYFRAMEWORK_API UIFInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	friend struct FIFItemContainer;
	friend struct FIFItemData;
protected:

	FIFItemEvent OnItemAddedEvent;
	FIFItemEvent OnItemUpdatedEvent;
	FIFItemEvent OnItemRemovedEvent;
	FIFOnInventoryChanged OnInventoryChanged;

	/* Array faking an database backend (where data is stored as json like DynamoDB, GameSparks, CosmosDB, MongoDB */
	TArray<TSharedPtr<FJsonObject>> FakeBackend;

	/* Key is deserialized from Json and it stored in backend. */
	//UPROPERTY()
	//	TMap<uint8, FIFItem> InventoryItems;


	UPROPERTY()
		TArray<FIFItemData> InventoryItems;
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
	
	inline FIFItemEvent& GetOnItemAdded() { return OnItemAddedEvent; }
	inline FIFItemEvent& GetOnItemUpdated() { return OnItemUpdatedEvent; }
	inline FIFItemEvent& GetOnItemRemoved() { return OnItemRemovedEvent; }

	inline uint8 GetMaxSlots()
	{
		return MaxSlots;
	}

	inline const FIFItemData& GetSlot(uint8 Idx)
	{
		return InventoryItems[Idx];;
	}

	inline UIFItemBase* GetItem(uint8 InLocalIndex)
	{
		return InventoryItems[InLocalIndex].Item;
		//return Inventory.Items[InLocalIndex].Item;
	}
	template<typename T>
	T* GetItem(uint8 InLocalIndex)
	{
		return Cast<T>(InventoryItems[InLocalIndex].Item);
		//return Cast<T>(Inventory.Items[InLocalIndex].Item);
	}

	TArray<uint8> GetLocalItemIdxs(TSubclassOf<UIFItemBase> ItemClass);

	template<typename T>
	TArray<T*> GetItems(TSubclassOf<T> ItemClass)
	{
		TArray<T*> Items;
		TArray<uint8> Idxs;
		for (uint8 Idx = 0; Idx < InventoryItems.Num(); Idx++)
		{
			if (InventoryItems[Idx].Item && InventoryItems[Idx].Item->IsA(ItemClass))
			{
				Items.Add(Cast<T>(InventoryItems[Idx].Item));
			}
		}

		return Items;
		//return Inventory.GetItems<T>(ItemClass);
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

	/* Adds item from class. Realistically you should never call it on client. */
	void AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromClass(const FSoftObjectPath& Item, uint8 InLocalIndex);
	
	void ServerAddItemFromClass_Implementation(FSoftObjectPath Item, uint8 InNetIndex);
	bool ServerAddItemFromClass_Validate(FSoftObjectPath Item, uint8 InNetIndex);
	UFUNCTION(BlueprintCallable, Category = "InventoryFramework")
		void BP_AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex);


	void AddItemFromEquipment(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromEquipment(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	void ServerAddItemFromEquipment_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	bool ServerAddItemFromEquipment_Validate(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	/*
		Confirm that change can be made and do the same change on client.
		We do not predict inventory modifications. Clients MUST wait for server to make changes and send confirmation back.
	*/
	UFUNCTION(Client, Reliable)
		void ClientAddItemFromEquipment(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	void ClientAddItemFromEquipment_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);

	void AddItemFromEquipmentAnySlot(class UIFEquipmentComponent* Source, uint8 SourceIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAddItemFromEquipmentAnySlot(class UIFEquipmentComponent* Source, uint8 SourceIndex);
	void ServerAddItemFromEquipmentAnySlot_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex);
	bool ServerAddItemFromEquipmentAnySlot_Validate(class UIFEquipmentComponent* Source, uint8 SourceIndex);
	/*
	Confirm that change can be made and do the same change on client.
	We do not predict inventory modifications. Clients MUST wait for server to make changes and send confirmation back.
	*/
	UFUNCTION(Client, Reliable)
		void ClientAddItemFromEquipmentAnySlot(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);
	void ClientAddItemFromEquipmentAnySlot_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex);


	//never call on clients.
	void AddItemAnySlot(class UIFItemBase* Source);
	/*
	Confirm that change can be made and do the same change on client.
	We do not predict inventory modifications. Clients MUST wait for server to make changes and send confirmation back.
	*/
	UFUNCTION(Client, Reliable)
		void ClientAddAnySlot(const FString& JsonData, uint8 InventoryIndex);
	void ClientAddAnySlot_Implementation(const FString& JsonData, uint8 InventoryIndex);


	virtual void OnItemAdded(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnItemChanged(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnItemRemoved(UIFItemBase* Item, uint8 LocalIndex) {};

	//these function are called on server.
	virtual void OnServerItemAdded(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnServerItemChanged(UIFItemBase* Item, uint8 LocalIndex) {};
	virtual void OnServerItemRemoved(UIFItemBase* Item, uint8 LocalIndex) {};

	void RemoveItem(uint8 InIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRemoveItem(uint8 InIndex);
	void ServerRemoveItem_Implementation(uint8 InIndex);
	bool ServerRemoveItem_Validate(uint8 InIndex);
	UFUNCTION(Client, Reliable)
		void ClientRemoveItem(uint8 Index);
	void ClientRemoveItem_Implementation(uint8 InIndex);


	UFUNCTION()
		void OnItemLoadedFreeSlot(TSoftClassPtr<class UIFItemBase> InItem);

	UFUNCTION()
		void OnItemLoaded(TSoftClassPtr<class UIFItemBase> InItem, uint8 InNetIndex);

	FSimpleMulticastDelegate& GetOnInventoryRead();
	

	UFUNCTION(Client, Reliable)
		void ClientSendJsonData(const FString& Data);
	void ClientSendJsonData_Implementation(const FString& Data);

	protected:
		void AddItem(TSoftClassPtr<class UIFItemBase> InItem, uint8 ItemIndex);
		TSharedPtr<FJsonObject> ItemToJson(FIFItemData* Item);
		FString JsonItemToString(TSharedPtr<FJsonObject> Object);
		void SendToBackend(TSharedPtr<FJsonObject> JsonObj, int32 Idx);

		FIFItemData JsonToItem(const FString& JsonString);
};
