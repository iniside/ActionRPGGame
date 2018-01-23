// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrAIController.h"
#include "SpectrBrainComponent.h"
bool operator>(float Rhs, const FSpectrDecision& Lhs)
{
	return Rhs > Lhs.Score;
}

ASpectrAIController::ASpectrAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpectrBrain = ObjectInitializer.CreateDefaultSubobject<USpectrBrainComponent>(this, TEXT("SpectrBrain"));
}