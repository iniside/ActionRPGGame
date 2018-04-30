// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFPeriodApplicationExtend.generated.h"

/**
 * If periodic effect of the same class already exists it duration will be extended. If not new effect
 * will be applied.
 */
UCLASS(meta = (DisplayName = "Periodic Extend"))
class ABILITYFRAMEWORK_API UAFPeriodApplicationExtend : public UAFEffectCustomApplication
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
		return true;
	}
};
