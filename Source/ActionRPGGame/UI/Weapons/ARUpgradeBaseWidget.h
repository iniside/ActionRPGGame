// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/ARWeaponBaseWidget.h"
#include "ARUpgradeBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARUpgradeBaseWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
public:
		TSoftClassPtr<class UARWeaponUpgradeItem> WeaponUpgrade;
	
	
};
