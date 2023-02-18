// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IFItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYFRAMEWORKUI_API UIFItemWidget : public UUserWidget
{
	GENERATED_BODY()
		friend class UIFItemContainerWidget;
public:
	TWeakObjectPtr<class UIFInventoryComponent> Inventory;
	/*
		Those indexes correponds to the client copy of items;
	*/

	UPROPERTY()
		uint8 NetIndex;
	UPROPERTY()
		uint8 LocalIndex;
	
	UPROPERTY(BlueprintReadWrite, Category = "InventoryFramework")
		UWidget* DragVisual;
public:
	virtual void OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item);

	virtual void OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item);

	virtual void OnItemRemoved(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Item Created"))
		void BP_OnItemCreated(class UIFItemBase* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Item Changed"))
		void BP_OnItemChanged(class UIFItemBase* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Item Removed"))
		void BP_OnItemRemoved(class UIFItemBase* Item);
};
