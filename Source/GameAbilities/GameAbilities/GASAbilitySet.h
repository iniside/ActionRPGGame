// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GASGlobals.h"
#include "GASAbilitySet.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GAMEABILITIES_API UGASAbilitySet : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		FGASAbilitySetContainer AbilitySet;
	
};
