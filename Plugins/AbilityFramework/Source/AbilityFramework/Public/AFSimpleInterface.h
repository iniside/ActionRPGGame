// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AFSimpleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAFSimpleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABILITYFRAMEWORK_API IAFSimpleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void ApplyEffect(const FGAEffectHandle& InHandle
		, const FGAEffect& InEffect) = 0;
};
