// Fill out your copyright notice in the Description page of Project Settings.

#include "STestAction_DropFirewood.h"
#include "AIController.h"
#include "SpectrBrainComponent.h"
#include "STestStorage.h"
#include "EngineUtils.h"

bool USTestAction_DropFirewood::NativeIsInRange(class AAIController* AIController)
{
	bool bInRange = false;
	if (TargetDropPoint)
	{
		APawn* Pawn = AIController->GetPawn();
		FVector Position = Pawn->GetActorLocation();
		FVector TargetPosition = TargetDropPoint->GetActorLocation();
		float Distance = FVector::Dist(Position, TargetPosition);
		if (Distance < MinDistance)
		{
			bInRange = true;
		}
	}
	return bInRange;
}
void USTestAction_DropFirewood::NativeMoveTo(class USpectrBrainComponent* Brain)
{
	Brain->MoveToActor(TargetDropPoint, MinDistance);
}
void USTestAction_DropFirewood::NativeOnMoveFinished(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	NativeExecute(InContext, AIController, Brain);
}
void USTestAction_DropFirewood::NativeExecute(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	NativeFinished();
}

bool USTestAction_DropFirewood::NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController)
{
	bool bFound = false;
	for (TActorIterator<ASTestStorage> ActorIt(AIController->GetWorld()); ActorIt; ++ActorIt)
	{
		TargetDropPoint = *ActorIt;
		bFound = true;
		break;
	}
	return bFound;
}