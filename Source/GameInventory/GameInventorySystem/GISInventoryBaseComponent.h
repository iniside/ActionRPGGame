// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISGlobalTypes.h"
#include "GameplayTags.h"
#include "GISInventoryBaseComponent.generated.h"

/*
	Simple event to force client, to redraw inventory after first load data has been repliated to client.
*/
/*
	Refactor delegates to be simple single cast delegates. We don't need them in blueprints.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGISOnInventoryLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnItemAdded, const FGISSlotUpdateData&, SlotUpdateInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnItemSlotSwapped, const FGISSlotSwapInfo&, SlotSwapInfo);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnTabSwapped, const FGISSlotSwapInfo&, SlotSwapInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGISOnCopyItemsToTab, const FGISSlotSwapInfo&, SlotSwapInfo);

/*
	We will use it, to reconstruct widget.
*/
DECLARE_MULTICAST_DELEGATE(FGISOnItemLooted);

DECLARE_DELEGATE_OneParam(FGISOnTabVisibilityChanged, int32);


DECLARE_DELEGATE_OneParam(FGISOnTabChanged, int32);
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
protected:

	/**
	 *	Should this inventory execute On Item Added To Slot function ?
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bExecuteOnItemAddedToSlot;
	/**
	 *	Should this inventory execute On Item Removed From Slot function ?
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bExecuteOnItemRemovedFromSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bCanDragItemsFromInventory;

	/**
	 *	Should this inventory execute On Item Added To Inventory function ?
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bExecuteOnItemAddedToInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		int32 MaximumActiveTabs;
	/*
		How much time must pass before tabs can be swapped again. Default 0.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		float TimeBetweenTabSwaps;
	/*
		Which item types, this component will accept.

		Useful if you have multiple inventory components on single actor, and want to make sure that they will accept
		only specific items.

		If nothing specified, nothing will be accepted, so make sure to add something!
	*/
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class UGISItemData>> AccepectedItems;



	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bReplicateTabsToOwnerOnly;

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
		Whether we should remove item from this inventory when we drag it to other inventory. Default true.
		Set it to false if you want item to stay in inventory. For example ability or skill inventory.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory Options")
		bool bRemoveItemsFromInvetoryOnDrag;

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
		Inventory Configuration.
	*/
	UPROPERTY(Editanywhere, Category = "Inventory")
		FGISInventoryConfiguration InventoryConfiguration;
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


	UPROPERTY(EditAnywhere, Category = "Loot Window")
		FGISLootConfiguration LootConfiguration;
	/*
		Add one full screen widget, which will act as drop area. 
		it should be added under all other widgets, so it will accept input as last.
		this should allow for dropping items out of inventory is
		straightforwad way, not some right clicking menus or other crap.
	*/
public:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		class UGISLootContainerBaseWidget* LootWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		class UGISContainerBaseWidget* InventoryContainer;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnInventoryLoaded OnInventoryLoaded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGISOnItemSlotSwapped OnItemSlotSwapped;

	/*
		CAlled when items from loot actor have been copied into temporary
		array, on inventory component.
	*/
	FGISOnItemLooted OnLootingStart;


	FGISOnTabVisibilityChanged OnTabVisibilityChanged;

	FGISOnTabChanged OnTabChanged;
public:
	UPROPERTY(ReplicatedUsing = OnRep_LootedItems)
		FGISPickupItemContainer LootFromPickup;

protected:
	/*
	Pickup actor that this inventory is interacting with.
	For now one inventory can interact with only one pickup actor.
	*/
	UPROPERTY(ReplicatedUsing = OnRep_PickupActor)
	class AGISPickupActor* CurrentPickupActor;

	UPROPERTY()
		APlayerController* PCOwner;

	UFUNCTION()
		void OnRep_PickupActor();



	UFUNCTION()
		void OnRep_LootedItems();

	UPROPERTY(ReplicatedUsing = OnRep_SlotUpdate, RepRetry)
		FGISSlotUpdateData SlotUpdateInfo;
	UFUNCTION()
		void OnRep_SlotUpdate();

	UPROPERTY(ReplicatedUsing = OnRep_SlotSwap, RepRetry)
		FGISSlotSwapInfo SlotSwapInfo;
	UFUNCTION()
		void OnRep_SlotSwap();

	UPROPERTY(ReplicatedUsing = OnRep_TabUpdated)
		FGISTabUpdateInfo TabUpdateInfo;
	UFUNCTION()
		void OnRep_TabUpdated();
public:

	virtual void InitializeComponent() override;
	virtual void PostInitProperties() override;

	virtual void InitializeWidgets(APlayerController* PCIn);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/*
		To easily get current inventory array for widget initialization.
	*/
	virtual FGISInventoryTab GetInventory();
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
		

	/* 
		Adds item to inventory. In Multiplayer, never call it on client.
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
		Override if you want constroll which functions from UGISItemInfo, are called
		when item is added to slot.
	 */
	virtual void OnItemAddedToSlot(class UGISItemData* AddedItemIn);

	/*
		Override if you want to specify custom conditions under which item can be added to slot.
	 */
	virtual bool CheckIfCanAddItemToSlot(class UGISItemData* TargetDataIn, int32 TargetTabIndex, int32 TargetSlotIndex,
		class UGISItemData* LastDataIn);

	/*
		Remove item from slot.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void RemoveItem(const FGISSlotInfo& TargetSlotType);

	void StartLooting(class AGISPickupActor* PickUp);

	/*
		Test function. Will loot everything from container!
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		virtual void LootItems();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void ServerLootItems();

	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		void LootOneItem(int32 ItemIndex);
	UFUNCTION(Server, Reliable, WithValidation)
		void SeverLootOneItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		void DropItemFromInventory(const FGISItemDropInfo& DropItemInfoIn);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropItemFromInventory(const FGISItemDropInfo& DropItemInfoIn);

	UFUNCTION(Client, Reliable)
		void ClientHideLootingWidget();

	void PostInventoryInitialized();
	
	/*
		Helper for item activation initem inventory if button has been pressed.
	*/
	virtual void InputSlotPressed(int32 TabIndex, int32 SlotIndex);
	/*
		Helper for item activation initem inventory if button has been released.
	*/
	virtual void InputSlotReleased(int32 TabIndex, int32 SlotIndex);
	/*
		Helper function if you want to copy pointer from slot, to another place.
		Useful if you for example don't want to activate item in slot, but 
		copy pointer to diffrent place, and activate it from here.
	 */
	inline UGISItemData* GetItemFromSlot(int32 TabIndex, int32 SlotIndex)
	{
		if (Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData)
		{
			return Tabs.InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData;
		}
		return nullptr;
	}

	inline void SetIsTabActive(int32 TabIndex, bool bIsTabActiveIn)
	{
		Tabs.InventoryTabs[TabIndex].bIsTabActive = bIsTabActiveIn;
	}

	inline FGISTabInfo& GetOneTab(int32 TabIndex)
	{
		return Tabs.InventoryTabs[TabIndex];
	}

	/*
		Swap item between tab. Make sure that tabs are of identical size.
		Otherwise you loose items from bigger tab!
	*/
	void SwapTabItems(int32 OriginalTab, int32 TargetTab);
	/*
		This will copy all item from OriginalTab, to TargetTab.
		All items in TargetTab will be replaced!

		Target tab must have AT LEAST the same amount of slots
		as original tab. Otherwise not all data will be copied.
	 */
	void CopyItemsToTab(int32 OriginalTab, int32 TargetTab);

	/**
	 *	Copy items from other Inventory Tab, to this inventory tab.
	 *	By default it will try to copy items only from single tab.
	 *	If items from this tab were copied before, it will try to 
	 *	copy items from next tab and so forth.
	 *
	 *	@param OtherIn - Other Inventory to copy from
	 *	@param TargetTabIndex - Index of tab in this inventory to copy to.
	 *
	 */
	void CopyItemsFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 TargetTabIndex);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerCopyItemsFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 TargetTabIndex);

	/**
	 *	Override if you want to perform on items any action, when they are copied from other inventory.
	 */
	virtual void OnCopyItemsFromOtherInventoryTab(class UGISItemData* DataIn);

	void CopyItemsToOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex);
	/**
	 *	Override if you want to perform on items any action, when they are copied  to other inventory.
	 */
	virtual void OnCopyItemsToOtherInventoryTab(class UGISItemData* DataIn);

	void CopyItemsFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex);

	int32 CountActiveTabs();

	/*1.
		Hotbar creation by:
		1. Create static Hotbar (always visible).
		2. Creating abitrary number of hidden Hotbar. They are not visible during playing game.
		3. You can edit hidden Hotbar in menu or something.
		4. When you swap Hotbar, pointer to contained items are copied from hidden Hotbar, to 
		static Hotbar.
	*/
	/*2.
		Another way.
		1. You create Hotbar like normal tabs.
		2. You set maximum number of tabs, which can be active at the same time.
		3. Create enough key mapping to support maximum amount of slots from all active Hotbar.
		4. So Hotbar which are swappable, will share exactly the same key mappings.
		5. When button is pressed it will check if current swappable hotbar is active.
		6. This means you can have only one active swappable hotbar at time.
	*/

	/*3.
		Or you can just create any number of hotbars, and assign to each of them unique
		key mappings if you want to the more WoW/RIFT/EQ etc like hotbars.
	*/
	/*
		2 and 3 are supported. The third.. Well, I'm still thinking about it.
	*/
	//it would be best if parameter here
	//would be dynamically generated list out of this component tabs.

	/**
	 *	Useful, when you want to create swappable hotbars (like GuildWars 2)
	 *	Or just swappable inventory tabs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		FGISTabInfo& BP_GetOneTab(int32 TabIndexIn);
	/*
		This function will need RPC call, since swapping is pretty much client side.
		As far as server and this component is concerned, swapping is just different way
		of displaying data.
		But in case of somethig like hotbar it might have gameplay implications.

		Another possible solution is to make sure that component can take care of swapping what is
		currently displayed, or at least make sure there is variable which tells how many tabs at once
		can be displayed and being active, to prevent cheating.
	*/
	/**
	*	Useful, when you want to prevent, activation of items on tab, which is not currently visible
	*	Similiar to how GuildWars 2 hotbar works.
	*	Need editor customization to make selecting tabs easier, than typing index.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		void BP_SetTabactive(int32 TabIndexIn, bool bIsTabActiveIn);
	/*
		There are three ways to do it. Operate only on client side, and ignore what server thinks about it.
		Since you will only be working on replicated version of inventory.

		Talk to server for each tab switch, and make server check if what player do is safe.

		Do it on server and client. If server result will be different from client, server will
		just override whatever happen.

		On the other hand things like bIsTabActive should be really set only on server for 
		something like action bar. If that's important part of gameplay that is.
	*/
	/**
	 *	Get current tab, hide it, and select next tab in it's place.
	 *	Like swappable hotbars from MMO.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		void BP_SetNextTab(int32 TabIndexIn);
	/**
	 *	Get single tab, from this inventory.
	 *	@param TabNameIn - name of tab to get;
	 *
	 *	@return Single tab from current inventory.
	 */
	FGISTabInfo& GetTabByName(FName TabNameIn);
	/**
	 *	Get single tab, from this inventory.
	 *	@param TabNameIn - name of tab to get;
	 *
	 *	@return Single tab from current inventory.
	 */
	UFUNCTION(BlueprintCallable, meta=(FriendlyName = "Get Tab By Name"), Category = "Game Inventory System")
		FGISTabInfo& BP_GetTabByName(FName TabNameIn);


	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;

	/*
		Invenory UObject replication support

	*/
private:
	void InitializeWidgets();
	void InitializeInventoryTabs();

	int32 LastTargetTab; //last tab from which we copied items, to this tab.
	int32 LastOtherOriginTab; //last tab from OTHER component, from which we copied items, to this component tab.
	UPROPERTY()
	class UGISInventoryBaseComponent* LastOtherOriginInventory;
public:
	inline int32 GetLastTargetTab() { return LastTargetTab; }
	inline int32 GetLastOtherOriginTab() { return LastOtherOriginTab; }
	inline UGISInventoryBaseComponent* GetLastOtherOriginInventory() { return LastOtherOriginInventory; }

	inline int32 GetSlotsInTab(int32 TabIndex) { return Tabs.GetSlotNum(TabIndex); };
	
	inline void CopyItemsFromTab(int32 TargetTab, int32 TargetIndex, int32 OriginTab, int32 OriginIndex) 
	{
		Tabs.SetItemData(TargetTab, TargetIndex, Tabs.GetItemData(OriginTab, OriginIndex));
	}
	inline class UGISItemData* GetItemDataInSlot(int32 TabIndex, int32 SlotIndex)
	{ 
		return Tabs.GetItemData(TabIndex, SlotIndex); 
	}
	inline void SetItemDataInSlot(int32 TabIndex, int32 SlotIndex, class UGISItemData* DataIn)
	{ 
		Tabs.SetItemData(TabIndex,SlotIndex, DataIn); 
	}
	inline bool IsInventoryValid(int32 TabIndex, int32 SlotIndex)
	{
		return Tabs.IsValid(TabIndex, SlotIndex);
	}
	inline int32 GetItemCount(int32 TabIndex)
	{
		return Tabs.GetItemCount(TabIndex);
	}
	inline void DecrementItemCount(int32 TabIndex)
	{
		Tabs.InventoryTabs[TabIndex].ItemCount--;
	}
	inline void IncrementItemCount(int32 TabIndex)
	{
		Tabs.InventoryTabs[TabIndex].ItemCount++;
	}
	inline TArray<FGISTabInfo>& GetInventoryTabs()
	{
		return Tabs.InventoryTabs;
	}
	inline TArray<FGISSlotInfo>& GetInventorySlots(int32 TabIndex)
	{
		return Tabs.InventoryTabs[TabIndex].TabSlots;
	}
	inline bool GetRemoveItemsOnDrag()
	{
		return bRemoveItemsFromInvetoryOnDrag;
	}

	inline FGameplayTagContainer& GetTabTags(int32 TabIndex)
	{
		return Tabs.InventoryTabs[TabIndex].Tags;
	}

	inline void SetLastTargetTab(int32 LastTargetTabIn) { LastTargetTab = LastTargetTabIn; }
	inline void SetLastOtherOriginTab(int32 LastOtherOriginTabIn) { LastOtherOriginTab = LastOtherOriginTabIn; }
	inline void SetLastOtherOriginInventory(UGISInventoryBaseComponent* LastOtherOriginInventoryIn) { LastOtherOriginInventory = LastOtherOriginInventoryIn; }

	inline class UGISContainerBaseWidget* GetInventoryWidget() { return InventoryContainer; }
};



