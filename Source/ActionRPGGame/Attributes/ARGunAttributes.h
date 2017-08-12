// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributesBase.h"
#include "ARGunAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARGunAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase BaseDamage;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase CritChance;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Magazine;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase RateOfFire;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase ReloadSpeed;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase HorizontalStability;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase VerticalStability;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase Spread;
	
	
};
