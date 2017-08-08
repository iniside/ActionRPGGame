// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARAbilityWidget.h"
#include "ARAbilitySlotConfigWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilitySlotConfigWidget : public UARAbilityWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Config")
		FGameplayTag InputBinding;
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry
			, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry
		, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
};
