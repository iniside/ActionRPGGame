// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARUMGWidgetBase.h"
#include "ARInventoryScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARInventoryScreenWidget : public UARUMGWidgetBase
{
	GENERATED_BODY()
public:
	
	TWeakObjectPtr<class UIFInventoryComponent> Inventory;
	TWeakObjectPtr<class UARUIComponent> UI;
	TWeakObjectPtr<class AARPlayerController> PC;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* RightWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* LeftWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* SideWeaponWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemWeaponWidget* BottomBackWeaponWidget;

	/*
		Contains list of items compatibile with selected slot.
	*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* SelectedItemsContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SelectedWeapon;
	
	void SetWeaponName(const FString& Name);

	void UpdateItemList(const TArray<uint8>& LocalIdxs);

};
