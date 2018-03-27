// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARAbilityWidget.h"
#include "AMTypes.h"
#include "ARAbilitySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilitySlotWidget : public UARAbilityWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup Group;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMSlot AbilitySlot;
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* CooldownBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* AbilityIcon;
public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetActivationRemainingTime();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetActivationRemainingTimeNormalized();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetActivationCurrentTime();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetActivationCurrentTimeNormalized();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetActivationEndTime();

	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetCooldownRemainingTime();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetCooldownRemainingTimeNormalized();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetCooldownCurrentTime();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetCooldownCurrentTimeNormalized();
	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		float GetCooldownEndTime();
	
	
};
