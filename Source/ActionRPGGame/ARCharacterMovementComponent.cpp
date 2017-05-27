// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "ARCharacterMovementComponent.h"


void UARCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
	//and now clamp velocity based on direction.
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector AccelerationDir = GetCurrentAcceleration().GetSafeNormal();
	float Dot = FVector::DotProduct(Forward, AccelerationDir);
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::FormatAsNumber(Dot));
	UE_LOG(LogTemp, Warning, TEXT("UARCharacterMovementComponent.CalcVelocity DotProduct: %f"), Dot);
	if (Dot < 0)
	{
		//backward
	}

	//else
	//cacl velocti based on dot, clamped minimum at MaxStarfingVelocity.
}

