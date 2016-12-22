// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GAGlobals.h"
#include "GAAbilitySet.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GAMEABILITIES_API UGAAbilitySet : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		FGameplayTag AbilityTag;
	UPROPERTY(EditAnywhere, Category = "Config")
		FGASAbilitySetContainer AbilitySet;
};
