// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemWidget.h"
#include "ARItemView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemView : public UIFItemWidget
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class UARUIInventoryComponent> InventoryComponent;

public:
	virtual void NativeConstruct() override;
	
	
	
};
