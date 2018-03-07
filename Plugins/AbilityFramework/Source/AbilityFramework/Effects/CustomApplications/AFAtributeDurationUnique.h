// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFAtributeDurationUnique.generated.h"

/**
 * Adds New Unique duration Effect. If effect of the same class is already applied to target, new effect will be ignored.
 */
UCLASS(meta = (DisplayName = "Duration Add Unique"))
class ABILITYFRAMEWORK_API UAFAtributeDurationUnique : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(
		const FGAEffectHandle& InHandle
		, const FGAEffect& EffectIn
		, struct FGAEffectContainer* InContainer
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());
	
	virtual bool CanApply(class IAFAbilityInterface* Target, TSubclassOf<UGAGameEffectSpec> EffectClass) override;
	 
	virtual bool ShowPeriod() override
	{
		return false;
	}
	virtual bool ShowDuration() override
	{
		return true;
	}
};
