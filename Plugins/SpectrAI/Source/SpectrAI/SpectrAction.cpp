// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrAction.h"
#include "SpectrBrainComponent.h"


void USpectrAction::NativeMoveTo(class USpectrBrainComponent* Brain)
{

}
void USpectrAction::Execute_Implementation(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain)
{

}
float USpectrAction::Score_Implementation(class USpectrContext* InContext, class AAIController* AIController)
{
	return Cost;
}

bool USpectrAction::EvaluateCondition_Implementation(class USpectrContext* InContext, class AAIController* AIController)
{
	return true;
}

void USpectrAction::NativeFinished()
{
	OwningBrain->OnActionFinished(this);
}