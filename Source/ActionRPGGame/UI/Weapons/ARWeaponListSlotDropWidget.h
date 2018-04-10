// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARWeaponBaseWidget.h"
#include "AMTypes.h"
#include "ARWeaponListSlotDropWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponListSlotDropWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup WeaponSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* IconImage;

	TWeakObjectPtr<class UARWeaponListWidget> WeaponList;
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry
		, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
