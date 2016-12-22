// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAttributesBase.h"
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
		FGAAttributeBase ActivationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase CooldownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase PeriodTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase Radius;

	/* Bonus to damage or healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase RawBonus;
	/* Bonus to damage or healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase PercentageBonus;

	/* Bonus to damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase RawBonusDamage;
	/* Bonus to damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase PercentageBonusDamage;

	/* Bonus to healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase RawBonusHeal;
	/* Bonus to healing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase PercentageBonusHeal;
	
};
