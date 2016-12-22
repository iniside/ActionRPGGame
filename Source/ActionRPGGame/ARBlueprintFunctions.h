// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARBlueprintFunctions.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARBlueprintFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Aim Offset")
		static FVector2D CalculateAimOffset(ACharacter* Character, FName Socket, const FVector& HitLocation);
	
	
};
