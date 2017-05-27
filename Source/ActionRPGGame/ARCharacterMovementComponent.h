// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GSCharacterMovementComponent.h"
#include "ARCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARCharacterMovementComponent : public UGSCharacterMovementComponent
{
	GENERATED_BODY()
public:
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MaxStrafingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MaxBackSpeed;
	
};
