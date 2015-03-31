#pragma once
#include "GameplayTagContainer.h"
#include "GISGlobalTypes.generated.h"

USTRUCT()
struct FGISInventoryConfiguration
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(Editanywhere, Category = "Inventory")
		FName DropSlottName;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		TSubclassOf<class UGISContainerBaseWidget> InventoryContainerClass;

	/*
	Type of tab used in this container. meta = (ExposeOnSpawn)
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TSubclassOf<class UGISTabBaseWidget> TabClass;

	/*
	Type of slot used in this container.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TSubclassOf<class UGISSlotBaseWidget> SlotClass;

	/*
	Type if item widget, which can be contained in slot.
	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
		TSubclassOf<class UGISItemBaseWidget> ItemClass;

	bool IsValid();
};

USTRUCT()
struct GAMEINVENTORYSYSTEM_API FGISLootSlotInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly)
	class UGISItemData* SlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> SlotComponent;
	UPROPERTY(BlueprintReadOnly)
	class AGISPickupActor* OwningPickupActor;

	FGISLootSlotInfo()
		: SlotData(nullptr) {};
	FGISLootSlotInfo(int32 SlotIndexIn, TWeakObjectPtr<class UGISInventoryBaseComponent> SlotComponentIn,
	class AGISPickupActor* OwningPickupActorIn)
		: SlotIndex(SlotIndexIn),
		SlotComponent(SlotComponentIn),
		OwningPickupActor(OwningPickupActorIn)
	{};
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FPickupItemData
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISPickupItemContainer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGISLootSlotInfo> Loot;
	
	UPROPERTY()
		int8 ForceRep;
};

//hmp. This actually doesn't lend itself as extensible solution...
UENUM(BlueprintType)
namespace EGISSlotType
{
	enum Type
	{
		Inventory,
		Hotbar,
		Character,

		MAX_INVALID //always last.
	};
}
USTRUCT()
struct GAMEINVENTORYSYSTEM_API FGISItemDropInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 TabIndex;
};

USTRUCT()
struct GAMEINVENTORYSYSTEM_API FGISSlotUpdateData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 TabIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly)
		class UGISItemData* SlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> SlotComponent;
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISSlotSwapInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int8 ReplicationCounter;
	UPROPERTY(BlueprintReadOnly)
		int32 LastTabIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 LastSlotIndex;
	UPROPERTY(BlueprintReadOnly)
		class UGISItemData* LastSlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> LastSlotComponent;
	UPROPERTY(BlueprintReadOnly)
		int32 TargetTabIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 TargetSlotIndex;
	UPROPERTY(BlueprintReadOnly)
		class UGISItemData* TargetSlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> TargetSlotComponent;
	UPROPERTY()
		bool bRemoveItemsFromInvetoryOnDrag;

	void LastAddItem(const FGISSlotSwapInfo& SwapInfo) const;
	void LastRemoveItem(const FGISSlotSwapInfo& SwapInfo) const;
	void TargetAddItem(const FGISSlotSwapInfo& SwapInfo) const ;
	void TargetRemoveItem(const FGISSlotSwapInfo& SwapInfo) const;
	FGISSlotSwapInfo()
	{};

	FGISSlotSwapInfo(const struct FGISSlotInfo& LastSlot, class UGISItemData* LastItemDataIn,
		const struct FGISSlotInfo& TargetSlot, class UGISItemData* TargetItemDataIn);
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISSlotInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 SlotTabIndex;
	UPROPERTY(BlueprintReadWrite)
	class UGISItemData* ItemData;

	UPROPERTY(BlueprintReadWrite)
		TWeakObjectPtr<class UGISInventoryBaseComponent> CurrentInventoryComponent;

	FGISSlotInfo()
	{}
	FGISSlotInfo(const FGISSlotUpdateData& Data)
		: SlotIndex(Data.SlotIndex),
		SlotTabIndex(Data.TabIndex),
		ItemData(Data.SlotData),
		CurrentInventoryComponent(Data.SlotComponent)
	{};

	void SetFromLast(const FGISSlotSwapInfo& LastData)
	{
		SlotIndex = LastData.LastTabIndex;
		SlotTabIndex = LastData.LastSlotIndex;
		ItemData = LastData.LastSlotData;
		CurrentInventoryComponent = LastData.LastSlotComponent;
	}

	void SetFromTarget(const FGISSlotSwapInfo& TargetData)
	{
		SlotIndex = TargetData.TargetSlotIndex;
		SlotTabIndex = TargetData.TargetTabIndex;
		ItemData = TargetData.TargetSlotData;
		CurrentInventoryComponent = TargetData.TargetSlotComponent;
	}

	bool IsValid();
	class UGISItemData* GetItemData();
	void SetItemData(class UGISItemData* DataIn);
	void DecrementItemCount();
	void IncrementItemCount();
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISTabInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
		int32 TabIndex;
	/**
	 *	If tab is not active it will not receive any inputs.
	 */
	UPROPERTY()
		bool bIsTabActive;

	UPROPERTY(BlueprintReadOnly)
		bool bAcceptItemsFromDragDrop;
	/**
	 *	Name of Tab.
	 */
	UPROPERTY(BlueprintReadOnly)
		FName TabName;

	/**
	 *	These tags must be present in Item OwnedTags container.
	 *	Otherwise item will not be accepted into this tab.
	 */
	FGameplayTagContainer Tags;
	/*
		Count of actual items in this tab.
	*/
	UPROPERTY()
		int32 ItemCount;

	UPROPERTY(BlueprintReadWrite)
		int32 NumberOfSlots;

	/**
	 *	Slots contained within this tab.
	 */
	UPROPERTY(BlueprintReadOnly)
		TArray<FGISSlotInfo> TabSlots;
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISInventoryTab
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 NumberOfTabs;

	UPROPERTY(BlueprintReadOnly)
		TArray<FGISTabInfo> InventoryTabs;

	inline class UGISItemData* GetItemData(int32 TabIndex, int32 SlotIndex)
	{
		return InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData;
	}
	inline void SetItemData(int32 TabIndex, int32 SlotIndex, class UGISItemData* DataIn)
	{
		InventoryTabs[TabIndex].TabSlots[SlotIndex].ItemData = DataIn;
	}
	inline int32 GetSlotNum(int32 TabIndex)
	{
		return InventoryTabs[TabIndex].TabSlots.Num();
	}
	inline int32 GetTabNum()
	{
		return InventoryTabs.Num();
	}
	inline bool IsValid(int32 TabIndex, int32 SlotIndex)
	{
		if (InventoryTabs.IsValidIndex(TabIndex))
			if (InventoryTabs[TabIndex].TabSlots.IsValidIndex(SlotIndex))
				return true;

		return false;
	}
	inline int32 GetItemCount(int32 TabIndex)
	{
		return InventoryTabs[TabIndex].ItemCount;
	}
};

USTRUCT()
struct FGISSlotsInTab
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		bool bIsTabActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NumberOfSlots;
};

USTRUCT()
struct FGISInventoryTabConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		bool bIsTabActive;

	/**
	 *	Name of Tab.
	 */
	UPROPERTY(EditAnywhere)
		FName TabName;
	/**
	 *	These tags must be present in Item OwnedTags container.
	 *	Otherwise item will not be accepted into this tab.
	 */
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer Tags;
	/**
	 *	Number of slots, that this tab contain.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NumberOfSlots;
};
USTRUCT()
struct FGISInventoryConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FGISInventoryTabConfig> TabConfigs;
};


USTRUCT()
struct FGISTabUpdateInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int8 ReplicationCounter; //make syre that we replicate new data.
	UPROPERTY()
		int8 TargetTabIndex;
	UPROPERTY()
		int8 OriginTabIndex;
};