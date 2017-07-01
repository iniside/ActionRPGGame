// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Effects/GACustomCalculation.h"
#include "GACustomCalculationTest.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGACustomCalculationTest : public UGACustomCalculation
{
	GENERATED_BODY()
	
public:
	virtual float NativeCalculateMagnitude(const FGAEffectHandle& HandleIn) override;
	
};
