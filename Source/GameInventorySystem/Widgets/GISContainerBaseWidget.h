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
public:
	virtual void InitializeContainer();
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

	/*
		Name of drop slot, which will contain ItemWidget.

	*/
	UPROPERTY(EditAnywhere, Category = "Inventory")
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

	//could make it virtual, but don't want to expose it to blueprint
	//ideally blueprint user shouldn't know that this even exist.
	//in anycase they are only for client side interaction

	UFUNCTION()
		void Widget_OnItemAdded(const FGISSlotUpdateData& SlotUpdateInfo);
	
	UFUNCTION()
	void Widget_OnItemSlotSwapped(const FGISSlotSwapInfo& SlotSwapInfo);

	void AddItem(const FGISSlotSwapInfo& SlotSwapInfo);
	void RemoveItem(const FGISSlotSwapInfo& SlotSwapInfo);

	UFUNCTION()
	void InitializeInventory();
};
