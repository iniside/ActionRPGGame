// Fill out your copyright notice in the Description page of Project Settings.

#include "STestAction_PickItemAxe.h"
#include "AIController.h"
#include "SpectrBrainComponent.h"
#include "STestAxePickup.h"
#include "EngineUtils.h"

bool USTestAction_PickItemAxe::NativeIsInRange(class AAIController* AIController)
{
	bool bInRange = false;
	if (TargetItem)
	{
		APawn* Pawn = AIController->GetPawn();
		FVector Position = Pawn->GetActorLocation();
		FVector TargetPosition = TargetItem->GetActorLocation();
		float Distance = FVector::Dist(Position, TargetPosition);
		if (Distance < MinDistance)
		{
			bInRange = true;
		}
	}
	return bInRange;
}
void USTestAction_PickItemAxe::NativeMoveTo(class USpectrBrainComponent* Brain)
{
	Brain->MoveToActor(TargetItem, MinDistance);
}
void USTestAction_PickItemAxe::NativeOnMoveFinished(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	NativeExecute(InContext, AIController, Brain);
}
void USTestAction_PickItemAxe::NativeExecute(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	NativeFinished();
}

bool USTestAction_PickItemAxe::NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController)
{
	bool bFound = false;
	for (TActorIterator<ASTestAxePickup> ActorIt(AIController->GetWorld()); ActorIt; ++ActorIt)
	{
		TargetItem = *ActorIt;
		bFound = true;
		break;
	}
	return bFound;
}