// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFAttributeDurationOverride.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFAttributeDurationOverride : public UAFEffectCustomApplication
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer);
	
	
	
};
