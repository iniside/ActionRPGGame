// Fill out your copyright notice in the Description page of Project Settings.

#include "STestAction_PickItemAxe.h"
#include "AIController.h"
#include "SpectrBrainComponent.h"
#include "STestAxePickup.h"
#include "EngineUtils.h"

void USTestAction_PickItemAxe::Execute_Implementation(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	APawn* Pawn = AIController->GetPawn();

	
	for (TActorIterator<ASTestAxePickup> ActorIt(AIController->GetWorld()); ActorIt; ++ActorIt)
	{
		TargetItem = *ActorIt;
		break;
	}
	FVector Position = Pawn->GetActorLocation();
	FVector TargetPosition = TargetItem->GetActorLocation();
	float Distance = FVector::Dist(Position, TargetPosition);
	if (Distance > MinDistance)
	{
		Brain->MoveToActor(TargetItem);
		return;
	}
}

bool USTestAction_PickItemAxe::EvaluateCondition_Implementation(class USpectrContext* InContext, class AAIController* AIController)
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