// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/AFEffectApplicationRequirement.h"
#include "AFAttributeStongerOverride.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFAttributeStongerOverride : public UAFEffectApplicationRequirement
{
	GENERATED_BODY()
public:
	virtual bool CanApply(const FGAEffectHandle& InHandle) override;
	
	
};
