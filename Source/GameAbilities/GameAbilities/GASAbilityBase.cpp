// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "Tasks/GASAbilityTask.h"
#include "GASAbilityBase.h"

UGASAbilityBase::UGASAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASAbilityBase::OnAbilityExecutedNative()
{
	OnAbilityExecuted();
}

void UGASAbilityBase::OnAbilityCancelNative()
{
	OnAbilityCancel();
}

void UGASAbilityBase::OnTaskInitialized(UGameplayTask& Task)
{
	if (UGASAbilityTask* task = Cast<UGASAbilityTask>(&Task))
	{
		task->Ability = this;
	}
}
UGameplayTasksComponent* UGASAbilityBase::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return nullptr;
}
/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnTaskActivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Started; %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Add(&Task);
}
/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Removed: %s in ability: "), *Task.GetName(), *GetName());
	ActiveTasks.Remove(&Task);
}
AActor* UGASAbilityBase::GetOwnerActor(const UGameplayTask* Task) const
{
	return nullptr;
}
AActor* UGASAbilityBase::GetAvatarActor(const UGameplayTask* Task) const
{
	return nullptr;
}