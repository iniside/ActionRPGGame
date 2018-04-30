// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/GAGameEffect.h"
#include "AFAbilityCooldownSpec.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class ABILITYFRAMEWORK_API UAFAbilityCooldownSpec : public UGAGameEffectSpec
{
	GENERATED_BODY()
public:
	UAFAbilityCooldownSpec();
	
	
};
