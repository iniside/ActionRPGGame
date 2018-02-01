// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARUMGWidgetBase.h"
#include "GameplayTags.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "AMTypes.h"
#include "ARAbilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityWidget : public UARUMGWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UAMAbilityManagerComponent* AbilityManager;

	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup Group;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMSlot AbilitySlot;
	UPROPERTY(EditAnywhere, Category = "Config")
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, Category = "Config")
		FGameplayTag AbilityTag;
	
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

	UFUNCTION(BlueprintPure, Category = "ActionRPGGame|UI|Abilities")
		UTexture2D* GetIcon();
};
