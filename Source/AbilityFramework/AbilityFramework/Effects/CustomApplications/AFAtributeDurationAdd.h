// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFAtributeDurationAdd.generated.h"

/**
 * Adds New duration Effect.
 */
UCLASS(meta = (DisplayName = "Duration Add"))
class ABILITYFRAMEWORK_API UAFAtributeDurationAdd : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer);
	
	virtual bool ShowPeriod() override
	{
		return false;
	}
	virtual bool ShowDuration() override
	{
		return true;
	}
};
