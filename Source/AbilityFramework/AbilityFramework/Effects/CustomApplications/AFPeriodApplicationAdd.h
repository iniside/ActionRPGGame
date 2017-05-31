// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFPeriodApplicationAdd.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFPeriodApplicationAdd : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	bool ApplyEffect(const FGAEffectHandle& InHandle, const struct FGAEffect& EffectIn,
		FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer) override;
	
	
	
};
