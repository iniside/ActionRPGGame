// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFPeriodicApplInfiniteOverride.generated.h"

/**
 * Adds infinite periodic effect. If effect of the same class already exists, it will be removed first.
 */
UCLASS(meta=(DisplayName = "Periodic Infinite Override"))
class ABILITYFRAMEWORK_API UAFPeriodicApplInfiniteOverride : public UAFEffectCustomApplication
{
	GENERATED_BODY()
	
	
		virtual void ExecuteEffect(
			const FGAEffectHandle& InHandle
			, const FGAEffect& EffectIn
			, struct FGAEffectContainer* InContainer
			, const FAFEffectParams& Params
			, const FAFFunctionModifier& Modifier = FAFFunctionModifier())
	{};
	
};
