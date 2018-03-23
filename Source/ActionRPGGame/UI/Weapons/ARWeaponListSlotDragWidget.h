// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapons/ARWeaponBaseWidget.h"
#include "Components/Image.h"
#include "ARWeaponListSlotDragWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponListSlotDragWidget : public UARWeaponBaseWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category = "Weapon Widget")
		int32 WeaponIdx;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* IconImage;

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry
		, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	inline int32 GetWeapon()
	{
		return WeaponIdx;
	}
	void OnItemAdded();
protected:
	void OnItemLoaded(FPrimaryAssetId InPrimaryAssetId);
};
