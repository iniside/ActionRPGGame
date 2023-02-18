// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OrionInterface.generated.h"

enum class EFCardinalDirection : uint8
{
	N = 0,
	E = 1,
	S = 2,
	W = 3
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOrionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ORIONANIMATION_API IOrionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual float GetAnimOrient() { return 0; }

	virtual float GetAnimOrientN() { return 0; }
	virtual float GetAnimOrientE() { return 0; }
	virtual float GetAnimOrientS() { return 0; }
	virtual float GetAnimOrientW() { return 0; }

	virtual EFCardinalDirection GetCardianlDirection() { return EFCardinalDirection::N; }
};
