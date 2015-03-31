#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISContainerBaseWidget.generated.h"
/*
	Base class for inventory widget container. It will contain inventory tabs. How and what you do
	with tabs is up to you. You might only have one tab displayed as list in full screen.

	Or you can have multiple tabs displayed in discreete moveable windows. 
	This base widgets only contain info, about available slots and tabs, but don't display anything.

	you must create the display part by either extendting this class in C++ or in Blueprint.
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISContainerBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
protected:

	UPROPERTY()
		FGISInventoryConfiguration Config;
	/*
		Type of tab used in this container. meta = (ExposeOnSpawn)
	*/
	UPROPERTY()
		TSubclassOf<class UGISTabBaseWidget> TabClass;

	/*
		Type of slot used in this container.
	*/
	UPROPERTY()
		TSubclassOf<class UGISSlotBaseWidget> SlotClass;

	/*
		Type if item widget, which can be contained in slot.
	*/
	UPROPERTY()
		TSubclassOf<class UGISItemBaseWidget> ItemClass;

	/*
		Name of drop slot, which will contain ItemWidget.
	*/
	UPROPERTY()
		FName DropSlottName;

	/**
		Inventory which owns this widget
	*/
	UPROPERTY(BlueprintReadOnly)
	class UGISInventoryBaseComponent* InventoryComponent;

	/*
		Base widget doesn't care how slots are displyed, it will just create list of slots
		upon creation, so you can then display it however you want, by making derived widget in 
		blueprint.
	*/
	/**
		List of tabs for this container. Probabaly should make it read only.
	*/
	UPROPERTY(BlueprintReadWrite)
		TArray<class UGISTabBaseWidget*> InventoryTabs;
public:
	virtual void InitializeContainer(const FGISInventoryConfiguration& ConfigIn);
protected:
	UFUNCTION()
		void Widget_OnItemAdded(const FGISSlotUpdateData& SlotUpdateInfo);
	
	UFUNCTION()
		void Widget_OnItemSlotSwapped(const FGISSlotSwapInfo& SlotSwapInfo);

	UFUNCTION()
		void Widget_OnTabVisibilityChanged(int32 TabIndex);

	UFUNCTION(BlueprintImplementableEvent)
		void OnTabChangedVisibility(int32 TabIndex);

	UFUNCTION()
		void InitializeInventory();

	/*
		Redraw tab, when it changed.
	*/
	UFUNCTION()
		void Widget_OnTabChanged(int32 TabIndexIn);

	UFUNCTION(BlueprintCallable, Category = "Game Inventory System")
		UGISTabBaseWidget* GetTabByName(FName TabNameIn);

public:
	void AddItem(const FGISSlotSwapInfo& SlotSwapInfo);
	void RemoveItem(const FGISSlotSwapInfo& SlotSwapInfo);

	int32 GetItemCount(int32 TabIndex);
	void IncrementItemCount(int32 TabIndex);
	void SetSlotInfo(int32 TabIndex, int32 SlotIndex, const FGISSlotInfo& SlotInfo);
	void SetSlotData(int32 TabIndex, int32 SlotIndex, class UGISItemData* DataIn);
	class UGISItemBaseWidget* GetItemWidget(int32 TabIndex, int32 SlotIndex);
	void UpdateItemWidget(class UGISItemBaseWidget* Item, const FGISSlotInfo& SlotInfo);

};
