// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFPeriodApplicationAdd.generated.h"

/**
 * Adds new periodic effect.
 */
UCLASS(meta = (DisplayName = "Periodic Add"))
class ABILITYFRAMEWORK_API UAFPeriodApplicationAdd : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	bool ApplyEffect(
		const FGAEffectHandle& InHandle
		, const FGAEffect& EffectIn
		, struct FGAEffectContainer* InContainer
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier()) override;

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
		return true;
	}
	
};
