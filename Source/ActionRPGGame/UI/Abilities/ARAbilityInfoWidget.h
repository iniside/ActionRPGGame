// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARUMGWidgetBase.h"
#include "GameplayTags.h"
#include "Abilities/GAAbilityBase.h"
#include "ARAbilityWidget.h"
#include "ARAbilityInfoWidget.generated.h"
/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityInfoWidget : public UARAbilityWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Ability")
		EAMSlot AbilitySlot;
	UPROPERTY(EditAnywhere, Category = "Ability")
		EAMGroup AbilityGroup;

	float GetRemainingCooldown() const;
};
