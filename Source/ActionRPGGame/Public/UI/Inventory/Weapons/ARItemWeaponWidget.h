// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ARItemView.h"
#include "ARItemWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemWeaponWidget : public UARItemView
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class UARUIInventoryComponent> InventoryComponent;
	TWeakObjectPtr<class UARWeaponInventoryComponent> WeaponInventory;

	int8 Index;

	TWeakObjectPtr<class UARItemWeapon> WeaponItem;
	UPROPERTY(Transient)
		UARItemTooltipView* WeaponTooltip;
public:
	virtual void NativeConstruct() override;
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
		void OnWeaponAdded2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem);
	UFUNCTION()
		void OnWeaponUpdated2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem);
	UFUNCTION()
		void OnWeaponRemoved2(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* InItem);
};
