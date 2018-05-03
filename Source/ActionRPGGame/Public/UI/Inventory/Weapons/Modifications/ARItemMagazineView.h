// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ARItemView.h"
#include "ARItemMagazineView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemMagazineView : public UARItemView
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
		void OnMagazineUpgradeInstalled(class UARItemWeapon* Weapon
			, class UARWeaponUpgradeItem* Upgrade
			, int8 WeaponIndex);
	UFUNCTION()
		void OnMagazineUpgradeRemoved(class UARItemWeapon* Weapon
			, class UARWeaponUpgradeItem* Upgrade
			, int8 WeaponIndex);
};
