// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "AFEffectCustomApplication.generated.h"

/**
 * Default application used for instant effects.
 */
UCLASS(meta = (DisplayName = "Default Application"))
class ABILITYFRAMEWORK_API UAFEffectCustomApplication : public UObject
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(
		  const FGAEffectHandle& InHandle
		, const FGAEffect& EffectIn
		, struct FGAEffectContainer* InContainer
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());
	
	virtual void ApplyExecute(
		  const FGAEffectHandle& InHandle
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	virtual bool CanApply(class IAFAbilityInterface* Target, TSubclassOf<UGAGameEffectSpec> EffectClass)
	{
		return true;
	}

	virtual bool ShowPeriod()
	{
		return false;
	}
	virtual bool ShowDuration()
	{
		return false;
	}
};
