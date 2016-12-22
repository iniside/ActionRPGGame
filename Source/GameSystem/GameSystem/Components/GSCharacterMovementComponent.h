// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "GSCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMESYSTEM_API UGSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		float CurrentAcceleration;

	float LastPosition;
protected:
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
};
