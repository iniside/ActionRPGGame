// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "AFEffectApplicationRequirement.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectApplicationRequirement : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CanApply(FGAEffect* EffectIn, FGAEffectProperty& InProperty)
	{
		return true;
	}
	
};
