// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GAPhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ABILITYFRAMEWORK_API UGAPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Fx")
		UParticleEmitter* SurfaceHitFX;
	
};
