// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARWeaponBaseWidget.h"
#include "AMTypes.h"
#include "ARWeaponManagerSlotDropWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponManagerSlotDropWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup WeaponSlot;
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry
		, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	
	
};
