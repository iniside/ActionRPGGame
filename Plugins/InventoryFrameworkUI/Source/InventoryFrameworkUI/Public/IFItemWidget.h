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
protected:
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

	void OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex);

	void OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Item Created"))
		void BP_OnItemCreated(class UIFItemBase* Item);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Item Changed"))
		void BP_OnItemChanged(class UIFItemBase* Item);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry
		, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)  override;
};
