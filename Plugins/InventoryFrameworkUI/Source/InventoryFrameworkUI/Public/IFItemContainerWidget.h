// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IFItemContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYFRAMEWORKUI_API UIFItemContainerWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
		TWeakObjectPtr<class UIFInventoryComponent> Inventory;
	
	UPROPERTY(BlueprintReadOnly, Category = "InventoryFramework")
		TArray<class UIFItemWidget*> InventoryWidgets;

	UPROPERTY(EditAnywhere, CAtegory = "Widgets")
		TSubclassOf<class UIFItemWidget> ItemClass;

public:
	void SetInventory(UIFInventoryComponent* InInventory);

	void CreateInventory();

	/*
		Called after all widgets in InvetoryWidgets array has been created.
	*/
	virtual void NativeOnInventoryCreated();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Inventory Created"))
		void BP_OnInventoryCreated();
};
