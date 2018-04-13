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

		TWeakObjectPtr<class UIFInventoryComponent> Inventory;
	
	TArray<class UIFItemWidget*> InventoryWidgets;

public:
	void SetInventory(UIFInventoryComponent* InInventory);

	void CreateInventory();
};
