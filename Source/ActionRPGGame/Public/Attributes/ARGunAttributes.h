// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributesBase.h"
#include "ARGunAttributes.generated.h"

/**
 * 1. Register FAFReplicationHandle with attribute change.
 * 2. If handle is valid then attribute change is also valid (for attribute prediction)
 * 3. If valid we don't override with server values.
 * 4. Wont work on attributes which can be changed by several sources at the same time like HP.
 * This will require custom treatment.
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
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Magazine, Category = "Base")
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
	
	UFUNCTION()
		void OnRep_Magazine(FAFAttributeBase OldVal);
};
