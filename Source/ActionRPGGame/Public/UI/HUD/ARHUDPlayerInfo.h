// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

#include "UI/ARUMGWidgetBase.h"
#include "ARHUDPlayerInfo.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARHUDPlayerInfo : public UARUMGWidgetBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* PlayerHealth;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* PlayerStamina;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* PlayerEnergy;
public:
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup001Slot001;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup001Slot002;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup001Slot003;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup002Slot001;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup002Slot002;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARAbilitySlotWidget* AbilityGroup002Slot003;

	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARWeaponSlotWidget* Weapon001;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARWeaponSlotWidget* Weapon002;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARWeaponSlotWidget* Weapon003;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//	UARWeaponSlotWidget* Weapon004;
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
};
