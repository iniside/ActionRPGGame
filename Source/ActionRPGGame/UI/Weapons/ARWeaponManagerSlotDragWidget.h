// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/ARWeaponBaseWidget.h"
#include "ARWeaponManagerSlotDragWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponManagerSlotDragWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category = "Weapon Widget")
		int32 WeaponIdx;
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	inline int32 GetWeapon()
	{
		return WeaponIdx;
	}
};
