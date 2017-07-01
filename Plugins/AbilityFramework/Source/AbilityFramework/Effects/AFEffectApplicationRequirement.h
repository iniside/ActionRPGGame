// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "AFEffectApplicationRequirement.generated.h"

/**
 * Default requirment always passes.
 */
UCLASS(meta=(DisplayName = "No Requirement"))
class ABILITYFRAMEWORK_API UAFEffectApplicationRequirement : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CanApply(FGAEffect* EffectIn, FGAEffectProperty& InProperty,
		struct FGAEffectContainer* InContainer,
		const FGAEffectContext& InContext,
		const FGAEffectHandle& InHandle)
	{
		return true;
	}
	
};
