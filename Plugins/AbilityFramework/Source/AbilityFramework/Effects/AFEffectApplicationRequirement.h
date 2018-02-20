// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "AFEffectApplicationRequirement.generated.h"

/**
 * Default requirment always passes.
 */
UCLASS(meta=(DisplayName = "No Requirement"))
class ABILITYFRAMEWORK_API UAFEffectApplicationRequirement : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CanApply(
		  const FGAEffect& EffectIn
		, const FAFEffectParams& Params
		, const FGAEffectHandle& InHandle
		, struct FGAEffectContainer* InContainer)
	{
		return true;
	}
	
};
