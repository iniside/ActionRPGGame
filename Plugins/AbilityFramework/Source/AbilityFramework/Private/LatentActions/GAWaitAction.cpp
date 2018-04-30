// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GAWaitAction.h"

UGAWaitAction::UGAWaitAction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	TickFunction.bCanEverTick = true;
	TickFunction.bStartWithTickEnabled = true;
	TickFunction.bAllowTickOnDedicatedServer = true;
	TickFunction.bRunOnAnyThread = true;
	SetFlags(RF_StrongRefOnFrame);
}

UGAWaitAction* UGAWaitAction::NewGAWaitAction(UObject* InTaskOwner, float Time)
{
	UGAWaitAction* MyTask = nullptr;/*NewTask<UGAWaitAction>(InTaskOwner, InTaskOwner);
	if (MyTask)
	{
		MyTask->Time = Time;
	}*/
	return MyTask;
}

void UGAWaitAction::Activate()
{
	OnInitialized.Broadcast();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString("ObjectName: ") + GetName());
	}
	if (TaskOwner && TaskOwner->GetWorld())
	{
		UWorld* World = TaskOwner->GetWorld();
		TimeStarted = World->GetTimeSeconds();

		// Use a dummy timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &UGAWaitAction::OnTimeFinish, Time, false);
	}
}
void UGAWaitAction::TickTask(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction)
{
	OnTick.Broadcast();
};
void UGAWaitAction::OnTimeFinish()
{
	OnFinish.Broadcast();
	EndTask();
}