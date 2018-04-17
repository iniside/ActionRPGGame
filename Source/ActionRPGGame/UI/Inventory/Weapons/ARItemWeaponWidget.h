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

public:
	virtual void NativeConstruct() override;
public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
