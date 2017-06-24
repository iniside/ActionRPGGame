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
	virtual bool ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
		const FGAEffectContext& InContext,
		const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	virtual void ExecuteEffect(const FGAEffectHandle& InHandle,
		FGAEffectProperty& InProperty,
		const FGAEffectContext& InContext,
		const FAFFunctionModifier& Modifier = FAFFunctionModifier())
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
