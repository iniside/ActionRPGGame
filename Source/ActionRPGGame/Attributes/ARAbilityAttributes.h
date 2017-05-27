// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Attributes/GAAttributesBase.h"
#include "ARAbilityAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase ActivationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase CooldownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase PeriodTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase Radius;

	/* Bonus to damage or healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase RawBonus;
	/* Bonus to damage or healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase PercentageBonus;

	/* Bonus to damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase RawBonusDamage;
	/* Bonus to damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase PercentageBonusDamage;

	/* Bonus to healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase RawBonusHeal;
	/* Bonus to healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase PercentageBonusHeal;
	
};
