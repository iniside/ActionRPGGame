// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARUMGWidgetBase.h"
#include "ARWeaponModificationView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponModificationView : public UARUMGWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UARItemMagazineView* MagazineUpgrade;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UButton* CloseUpgradeView;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
		class UWrapBox* UpgradeList;

	virtual void NativeConstruct() override;
	void StartModifyWeapon(class UARItemWeapon* WeaponItem);
	void StopModifyWeapon();

	UFUNCTION()
		void CloseModificationView();
	
};
