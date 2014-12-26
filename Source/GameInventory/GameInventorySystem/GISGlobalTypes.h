#pragma once

#include "GISGlobalTypes.generated.h"

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FPickupItemData
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FPickupItemContainer
{
	GENERATED_USTRUCT_BODY()
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
struct GAMEINVENTORYSYSTEM_API FGISLootSlotInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 SlotIndex;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISItemData> SlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> SlotComponent;
	UPROPERTY(BlueprintReadOnly)
		class AGISPickupActor* OwningPickupActor;
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
		TWeakObjectPtr<class UGISItemData> SlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> SlotComponent;
};

USTRUCT(BlueprintType)
struct GAMEINVENTORYSYSTEM_API FGISSlotSwapInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 LastTabIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 LastSlotIndex;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISItemData> LastSlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> LastSlotComponent;
	UPROPERTY(BlueprintReadOnly)
		int32 TargetTabIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 TargetSlotIndex;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISItemData> TargetSlotData;
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> TargetSlotComponent;
	UPROPERTY()
		bool bRemoveItemsFromInvetoryOnDrag;
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
		TWeakObjectPtr<class UGISItemData> ItemData;

	UPROPERTY(BlueprintReadWrite)
		TWeakObjectPtr<class UGISInventoryBaseComponent> CurrentInventoryComponent;
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
		bool bIsTabVisible;

	int32 LinkedTab;

	UPROPERTY(BlueprintReadWrite)
		int32 NumberOfSlots;

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

	UPROPERTY(EditAnywhere)
		bool bIsTabVisible;

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

	UPROPERTY(EditAnywhere)
		bool bIsTabVisible;

	/*
		To which tab this, tab is linked ? -1 to none. Otherwise specific Index of linked tab.
		This is essentialy very simple Linked List. So you should just specify index which is next
		to current tab. If this tab is last, just specifi index of tab which is first. Usually 0.

		This linked list is used for swapping tab using hotkeys or other user defined action. It makes
		easier to determine which tab should be displayed after current one, if you opt, to display only
		one at time.
	*/
	UPROPERTY(EditAnywhere)
		int32 LinkedToTab;

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