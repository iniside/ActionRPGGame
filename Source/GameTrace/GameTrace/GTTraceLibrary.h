// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GTTraceLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMETRACE_API UGTTraceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static	FHitResult LineTraceByChannel(bool bDrawDebug,
			bool bDrawCorrectedDebug,
			bool bUseCorrectedTrace,
			float Range);
	
	
};
