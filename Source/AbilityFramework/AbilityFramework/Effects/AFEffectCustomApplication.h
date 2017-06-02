// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "AFEffectCustomApplication.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectCustomApplication : public UObject
{
	GENERATED_BODY()
public:
	virtual bool ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer);
	
	
};
