// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARItemBase.h"
#include "UI/Inventory/ARItemView.h"
#include "ARItemTooltipView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemTooltipView : public UARItemView
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ActionRPGGame|UI")
		void OnTooltipCreated(const TArray<FARItemTooltipData>& OutItems);
	
	
};
