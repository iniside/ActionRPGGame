// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrAIController.h"
#include "SpectrBrainComponent.h"


ASpectrAIController::ASpectrAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpectrBrain = ObjectInitializer.CreateDefaultSubobject<USpectrBrainComponent>(this, TEXT("SpectrBrain"));
	AIPerception = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("AIPerception"));
}