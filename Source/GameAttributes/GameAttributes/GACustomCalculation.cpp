// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GACustomCalculation.h"

UGACustomCalculation::UGACustomCalculation(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
float UGACustomCalculation::CalculateMagnitude_Implementation(AActor* Instigator, class UGAAttributeComponent* InstigatorComponent,
	AActor* Target, class UGAAttributeComponent* TargetComponent)
{
	return 0;
}