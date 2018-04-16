// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemContainerWidget.h"
#include "ARWeaponContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponContainerWidget : public UIFItemContainerWidget
{
	GENERATED_BODY()
public:
		void InitializeWeaponItems(class UARUIComponent* UIComponent);
	
	
};
