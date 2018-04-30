// Fill out your copyright notice in the Description page of Project Settings.

#include "SpectrAction.h"
#include "SpectrBrainComponent.h"


void USpectrAction::NativeMoveTo(class USpectrBrainComponent* Brain)
{
	MoveTo(Brain);
}
void USpectrAction::NativeOnMoveFinished(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain)
{
	OnMoveFinished(InContext, AIController, Brain);
}
void USpectrAction::FinishMove(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain)
{
	NativeOnMoveFinished(InContext, AIController, Brain);
}
void USpectrAction::MoveTo_Implementation(class USpectrBrainComponent* Brain)
{

}

float USpectrAction::NativeScore(class USpectrContext* InContext, class AAIController* AIController)
{
	return Score(InContext, AIController);
}

float USpectrAction::Score_Implementation(class USpectrContext* InContext, class AAIController* AIController)
{
	return Cost;
}
bool USpectrAction::NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController)
{
	return EvaluateCondition(InContext, AIController);
}
bool USpectrAction::EvaluateCondition_Implementation(class USpectrContext* InContext, class AAIController* AIController)
{
	return true;
}

void USpectrAction::NativeFinished()
{
	OwningBrain->OnActionFinished(this);
}

void USpectrAction::NativeExecute(class USpectrContext* InContext, class AAIController* AIController, class USpectrBrainComponent* Brain)
{
	OnExecute(InContext, AIController, Brain);
}

void USpectrAction::ActionFinished()
{
	NativeFinished();
}

void USpectrAction::MoveToTarget(AActor* Target, float MinimumDistance)
{
	OwningBrain->MoveToActor(Target, MinimumDistance);
}