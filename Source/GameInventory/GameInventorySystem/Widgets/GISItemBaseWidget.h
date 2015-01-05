#pragma once
#include "Blueprint/UserWidget.h"
#include "../GISGlobalTypes.h"
#include "GISItemBaseWidget.generated.h"
/*
	This is base widget which will contain actuall client side data about item.
	Like last slot info, current slot info, etc.

	Also it will display data for items. Like tooltips, icons, names
*/
UCLASS()
class GAMEINVENTORYSYSTEM_API UGISItemBaseWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	/**
		User for Drag&Drop, indicate information about slot, from which this item was dragged off.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGISSlotInfo LastSlotInfo;

	/**
		User for Drag&Drop, indicate information about slot to which this item will be dropped.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGISSlotInfo TargetSlotInfo;

	UPROPERTY()
		class UGISItemData* ItemData;

	UPROPERTY()
		class UGISInventoryBaseComponent* InventoryComponent;
	
	virtual void InitializeItem();

	/*
		Probabaly can change them to pure.	
	*/
	/**
		Get item name to display
	*/
	UFUNCTION(BlueprintCallable, Category = "Item Information")
		FText GetItemName() const;

	UFUNCTION(BlueprintCallable, Category = "Item Information")
		UTexture2D* GetIcon() const;

	UFUNCTION(BlueprintCallable, Category = "Item Information")
		FText GetDescription() const;
};
