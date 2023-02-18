// Fill out your copyright notice in the Description page of Project Settings.

#include "STestAction_ChopFirewood.h"
#include "AIController.h"
#include "SpectrBrainComponent.h"
#include "STestTree.h"
#include "EngineUtils.h"

bool USTestAction_ChopFirewood::NativeIsInRange(class AAIController* AIController)
{
	//We can evaluate again here if we have target, and if it worth going.
	//we can either find better fit or abort plan at this point.
	bool bInRange = false;
	if (TargetTree)
	{
		APawn* Pawn = AIController->GetPawn();
		FVector Position = Pawn->GetActorLocation();
		FVector TargetPosition = TargetTree->GetActorLocation();
		float Distance = FVector::Dist(Position, TargetPosition);
		if (Distance < MinDistance)
		{
			bInRange = true;
		}
	}
	return bInRange;
}
void USTestAction_ChopFirewood::NativeMoveTo(class USpectrBrainComponent* Brain)
{
	Brain->MoveToActor(TargetTree, MinDistance);
}
void USTestAction_ChopFirewood::NativeOnMoveFinished(class USpectrContext* InContext
	, class AAIController* AIController
	, class USpectrBrainComponent* Brain)
{
	NativeExecute(InContext, AIController, Brain);
}
//void USTestAction_ChopFirewood::NativeExecute(class USpectrContext* InContext
//	, class AAIController* AIController
//	, class USpectrBrainComponent* Brain)
//{
//	NativeFinished();
//}

bool USTestAction_ChopFirewood::NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController)
{
	bool bFound = false;
	for (TActorIterator<ASTestTree> ActorIt(AIController->GetWorld()); ActorIt; ++ActorIt)
	{
		TargetTree = *ActorIt;
		bFound = true;
		break;
	}
	return bFound;
}