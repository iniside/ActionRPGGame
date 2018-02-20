// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFPeriodApplicationInfiniteAdd.generated.h"

/**
 * Periodic effect will be applied for infinite amount of time.
 */
UCLASS(meta = (DisplayName = "Periodic Infinite Add"))
class ABILITYFRAMEWORK_API UAFPeriodApplicationInfiniteAdd : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(
		const FGAEffectHandle& InHandle
		, const FGAEffect& EffectIn
		, struct FGAEffectContainer* InContainer
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	virtual void ExecuteEffect(
		const FGAEffectHandle& InHandle
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier())
	{};

	virtual bool ShowPeriod() override
	{
		return true;
	}
	virtual bool ShowDuration() override
	{
		return false;
	}
	
};
