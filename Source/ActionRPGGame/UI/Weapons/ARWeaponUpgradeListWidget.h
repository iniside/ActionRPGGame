// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARWeaponBaseWidget.h"
#include "ARWeaponUpgradeListWidget.generated.h"

/**
 * Contains list of upgrades for weapon, for selected slot.
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponUpgradeListWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY()
		TArray<UARUpgradeBaseWidget*> UpgradeList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* ItemContainer;
public:
	void OnShow();
};
