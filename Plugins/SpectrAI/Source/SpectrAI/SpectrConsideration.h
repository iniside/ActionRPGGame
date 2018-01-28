// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpectrConsideration.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class SPECTRAI_API USpectrConsideration : public UObject
{
	GENERATED_BODY()
	
public:
	virtual float Score() const { return 0; }
	
};
