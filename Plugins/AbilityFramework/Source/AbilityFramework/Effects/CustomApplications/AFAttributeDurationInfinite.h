// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFAttributeDurationInfinite.generated.h"

/**
 * Adds infinite duration effect.
 */
UCLASS(meta = (DisplayName = "Duration Infinite Add"))
class ABILITYFRAMEWORK_API UAFAttributeDurationInfinite : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(
		const FGAEffectHandle& InHandle
		, const FGAEffect& EffectIn
		, struct FGAEffectContainer* InContainer
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());
	
	virtual bool ShowPeriod() override
	{
		return false;
	}
	virtual bool ShowDuration() override
	{
		return false;
	}
	
};
