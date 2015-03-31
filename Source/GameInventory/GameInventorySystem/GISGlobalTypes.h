#pragma once
#include "GameplayTagContainer.h"
#include "GISGlobalTypes.generated.h"

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

	bool IsValid();
	class UGISItemData* GetItemData();
	void SetItemData(class UGISItemData* DataIn);
	void DecrementItemCount();
	void IncrementItemCount();
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

	FGISSlotSwapInfo()
	{};

	FGISSlotSwapInfo(const FGISSlotInfo& LastSlot, class UGISItemData* LastItemDataIn,
		const FGISSlotInfo& TargetSlot, class UGISItemData* TargetItemDataIn)
		: LastTabIndex(LastSlot.SlotTabIndex),
		LastSlotIndex(LastSlot.SlotIndex),
		LastSlotComponent(LastSlot.CurrentInventoryComponent),
		LastSlotData(LastItemDataIn),
		TargetTabIndex(TargetSlot.SlotTabIndex),
		TargetSlotIndex(TargetSlot.SlotIndex),
		TargetSlotComponent(TargetSlot.CurrentInventoryComponent),
		TargetSlotData(TargetItemDataIn)
	{}
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