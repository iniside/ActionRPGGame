// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../GAGlobalTypes.h"
#include "AFEffectCustomStackingRule.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFEffectCustomStackingRule : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool CanStack(class UAFAbilityComponent* InComp, struct FGAEffectContainer* InContainer,
		const FGAEffectHandle& InHandle);
	
};
