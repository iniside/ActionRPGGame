// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISGlobalTypes.h"
#include "GameplayTags.h"
#include "GISInventoryBaseComponent.generated.h"

/*
	Simple event to force client, to redraw inventory after first load data has been repliated to client.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGISOnInventoryLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnItemAdded, const FGISSlotUpdateData&, SlotUpdateInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnItemSlotSwapped, const FGISSlotSwapInfo&, SlotSwapInfo);

/*
	We will use it, to reconstruct widget.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGISOnItemLooted);

/**
	Note to self. I could probabaly use GameplayTags, to determine exactly how interaction between
	various items, and components should interact.

	Though of course setting up gameplay tags is bit of hassle, they should be able to give
	very fine controll over interactions.



	Another thing. I don't like the fact that inventory and other things are updated by OnRep_ functions.
	They work, the problem is that state of client side inventory is not updated until OnRep function is fired.

	I have to add some simple client side prediction. Especially for looting. Here it can easily cause out of bounds 
	errors for array if UI is updated to late and player click on item, which is not already in inventory.

	Or.. I can just check for valid index in array.
*/

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEINVENTORYSYSTEM_API UGISInventoryBaseComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Options")
		TArray <FGISSlotsInTab> InitialTabInfo;

	/*
		Which item types, this component will accept.

		Useful if you have multiple inventory components on single actor, and want to make sure that they will accept
		only specific items.

		If nothing specified, nothing will be accepted, so make sure to add something!
	*/
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class UGISItemData>> AccepectedItems;

	/*
	Indicates if items can be activated directly in invetory window.
	Useful if you want to prevent player from activating items in invetory. For example
	healing potions, or something.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bCanActivateItemInInventory;

	/*
		Max distance from which you can pickup items in this inventory.
		If player is further than this value, looting window will automatically close.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Options")
		float MaxLootingDistance;

	/*
		RepRetry - inventory is in general to important it must be replicated!
		And besides that it's not replicated very often.
	*/
	/*
		Each element in Array InventoryTabs is single tab.
		NumberOfSlots - Number of inventory slots in this tab.
		TabIndex - easily accessible Index of this tab.
		TabSlots<> array of inventory Slots in this Tab.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_InventoryCreated, RepRetry, Category = "Inventory")
		FGISInventoryTab Tabs;

	UFUNCTION()
		void OnRep_InventoryCreated();
	/*

	*/
	/**
	The idea behind tags, while might seem to be complicated at first, is actually very, very
	easy.

	When item is dropped into slot, component will check OwnedTags from component which item
	has been dragged against RequredTags of component to which item is droped.

	If OwnedTags does NOT contain any tags, which is in RequiredTags, drop operation cannot be
	compoleted, so make sure to setup your tags properly.

		Tags, which this componenet have,
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		FGameplayTagContainer OwnedTags;

	/**
		Items from another component, must have these tags to be added to this component.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, Instanced)
		TSubclassOf<class UGISContainerBaseWidget> InventoryContainerClass;

	/*
		This is very bad pack. When componeents will work with normal objects (pointers)
		It should be replaced with class UGISLootContainerBaseWidget* LootWidget;
	*/
	UPROPERTY(EditAnywhere, Instanced)
		TSubclassOf<class UGISLootContainerBaseWidget> LootWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UGISLootContainerBaseWidget* LootWidget;

	UPROPERTY(BlueprintReadOnly)
		class UGISContainerBaseWidget* InventoryContainer;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnInventoryLoaded OnInventoryLoaded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnItemSlotSwapped OnItemSlotSwapped;

	UPROPERTY()
		FGISOnItemLooted OnItemLooted;
private:
	/*
		Pickup actor that this inventory is interacting with. 
		For now one inventory can interact with only one pickup actor.
	*/
	UPROPERTY(ReplicatedUsing=OnRep_PickupActor)
	class AGISPickupActor* CurrentPickupActor;

	UFUNCTION()
		void OnRep_PickupActor();
	UPROPERTY(ReplicatedUsing = OnRep_LootedItems)
		TArray<UGISItemData*> LootedItems;
	UFUNCTION()
		void OnRep_LootedItems();
	UFUNCTION()
	void ConstructLootPickingWidget();

private:
	UPROPERTY(ReplicatedUsing = OnRep_SlotUpdate, RepRetry)
	FGISSlotUpdateData SlotUpdateInfo;
	UFUNCTION()
		void OnRep_SlotUpdate();

	UPROPERTY(ReplicatedUsing = OnRep_SlotSwap, RepRetry)
	FGISSlotSwapInfo SlotSwapInfo;
	UFUNCTION()
		void OnRep_SlotSwap();
public:

	virtual void InitializeComponent() override;
	virtual void PostInitProperties() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/*
		To easily get current inventory array for widget initialization.
	*/
	virtual FGISInventoryTab GetInventoryTabs();
	/*
		Initial draft for picking up items from world.


		First you need to find item. This part is somewhat game specific, you need use radial sweep,
		line trace etc, to find items.

		The important part, is the fact that trace should happen ON SERVER, to make sure that
		item is in that place.

		Once item has been found, we will call PickItem function and pass Actor pointer to it.
		There is no reason to use UObject as UObjects can't be placed on level.

		Pickable actor should implement special interface, 
		which I have not yet decided how it will look ;/.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game InventorySystem")
		void PickItem(AActor* PickupItemIn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPickItem(AActor* PickupItemIn);
		

	/* some changes for git.
	Testing Function1
	Technically you never should call this function from client, in client-server environment. NEVER
	EVER.

	It's here now just for testing. Once invetory plugin is fully operational, I will remove it.
	and make it authority callable only.
	*/
	UFUNCTION(BlueprintCallable, Category="Game Inventory System")
		virtual void AddItemToInventory(class UGISItemData* ItemIn);

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerAddItemToInventory(class UGISItemData* ItemIn);


	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void AddItemOnSlot(const FGISSlotInfo& TargetSlotType, const FGISSlotInfo& LastSlotType);

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerAddItemOnSlot(const FGISSlotInfo& TargetSlotType, const FGISSlotInfo& LastSlotType);
	
	/*
		Remove item from slot.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void RemoveItem(const FGISSlotInfo& TargetSlotType);

	/*
		Test function. Will loot everything from container!
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void LootItems();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerLootItems();

	/*
		Technically you should call it on server, since you should trace for it only
		on server ;). 
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void GetLootContainer(class AGISPickupActor* LootContainer);
	/*
		So in reality this function should never get called.
		But if for some reason, you really want to trace on client.. Here you go.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerGetLootContainer(class AGISPickupActor* LootContainer);

	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
	void LootOneItem(int32 ItemIndex);
	UFUNCTION(Server, Reliable, WithValidation)
	void SeverLootOneItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		void DropItemFromInventory(const FGISItemDropInfo& DropItemInfoIn);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropItemFromInventory(const FGISItemDropInfo& DropItemInfoIn);
	/*
		Heyyy client, would you be so nice, and update this slot with this item ? Thanks!

		On more serious note, it will just call OnItemAdded delegate on client.
	*/
	UFUNCTION(Client, Reliable)
		void ClientUpdateInventory(const FGISSlotUpdateData& SlotUpdateInfoIn);


	UFUNCTION(Client, Reliable)
		void ClientSlotSwap(const FGISSlotSwapInfo& SlotSwapInfoIn);

	UFUNCTION(Client, Reliable)
		void ClientLoadInventory();

	UFUNCTION(Client, Reliable)
		void ClientReconstructLootWidget();

	UFUNCTION(Client, Reliable)
		void ClientConstructWidget();

	UFUNCTION(Client, Reliable)
		void ClientHideLootingWidget();

	void PostInventoryInitialized();


	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;

	/*
		Invenory UObject replication support

	*/
private:
	void InitializeInventoryTabs();
};



