// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/ARUpgradeBaseWidget.h"
#include "ARMagazineUpgradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARMagazineUpgradeWidget : public UARUpgradeBaseWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Icon;
	
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
