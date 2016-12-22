// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystem.h"
#include "GSCharacterMovementComponent.h"




void UGSCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	float CurrentPosition = UpdatedComponent->GetComponentLocation().Size();
	float OldVelocity = Velocity.Size();
	CurrentAcceleration = CurrentAcceleration + MaxAcceleration * deltaTime;
	UCharacterMovementComponent::PhysWalking(deltaTime, Iterations);
	float VelDifference = (Velocity.Size() - OldVelocity);
	float Difference = CurrentPosition - LastPosition;
	LastPosition = CurrentPosition;

	//CurrentAcceleration = Difference / deltaTime;
	CurrentAcceleration += FMath::Clamp<float>((VelDifference / deltaTime), 0, MaxAcceleration);
}