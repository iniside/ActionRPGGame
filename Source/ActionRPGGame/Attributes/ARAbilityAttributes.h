// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributesBase.h"
#include "ARAbilityAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase CastTime;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Cooldown;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase BaseDamage;
	
	
	
};
