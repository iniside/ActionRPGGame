// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask.h"

UGAAbilityTask::UGAAbilityTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bTickingTask = false;
	bSimulatedTask = false;
	bIsSimulating = false;
	bOwnedByTasksComponent = false;
	bClaimRequiredResources = true;
	TaskState = EGameplayTaskState::Uninitialized;
	ResourceOverlapPolicy = ETaskResourceOverlapPolicy::StartOnTop;
	Priority = FGameplayTasks::DefaultPriority;

	SetFlags(RF_StrongRefOnFrame);
}
//UWorld* UGAAbilityTask::GetWorld() const
//{
//	return GetOuterUGAAbilityBase()->GetWorld();
//}

void UGAAbilityTask::Initialize()
{

}

