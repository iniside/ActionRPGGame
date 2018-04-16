// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemWidget.h"
#include "ARItemWeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemWeaponWidget : public UIFItemWidget
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class UARWeaponInventoryComponent> WeaponInventory;
	TWeakObjectPtr<class UARUIComponent> UI;
	TWeakObjectPtr<class UARInventoryScreenWidget> InventoryWidget;
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	
};
