// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFAttributeDurationOverride.generated.h"

/**
 * If effect of the same class is already applied it will be removed, and new one will be applied.
 */
UCLASS(meta = (DisplayName = "Duration Override"))
class ABILITYFRAMEWORK_API UAFAttributeDurationOverride : public UAFEffectCustomApplication
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
		return true;
	}
	
};
