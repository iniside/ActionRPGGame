// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../../AbilityFramework.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask.h"

UGAAbilityTask::UGAAbilityTask(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsReplicated = false;
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



void UGAAbilityTask::EndAbilityTask()
{

}

//int32 UGAAbilityTask::GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack)
//{
//	if (HasAnyFlags(RF_ClassDefaultObject))
//	{
//		return FunctionCallspace::Local;
//	}
//	check(AbilityComponent.IsValid());
//	return AbilityComponent->GetFunctionCallspace(Function, Parameters, Stack);
//}
//bool UGAAbilityTask::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
//{
//	check(!HasAnyFlags(RF_ClassDefaultObject));
//	check(AbilityComponent.IsValid());
//
//
//	UNetDriver* NetDriver = AbilityComponent->GetOwner()->GetNetDriver();
//	if (NetDriver)
//	{
//		NetDriver->ProcessRemoteFunction(AbilityComponent->GetOwner(), Function, Parameters, OutParms, Stack, this);
//		return true;
//	}
//
//	return false;
//}