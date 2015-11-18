// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Net/UnrealNetwork.h"
#include "GameAbilities.h"
#include "Tasks/GASAbilityTask.h"
#include "GASAbilityBase.h"

UGASAbilityBase::UGASAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAbilityExecuting = false;
	bIsOnCooldown = false;
	bReplicate = true;
	bIsNameStable = false;
}

void UGASAbilityBase::InitAbility()
{
	if (OwningComp.IsValid())
	{
		World = OwningComp->GetWorld();
	}
}

void UGASAbilityBase::OnAbilityExecutedNative()
{
	if (Cooldown > 0)
	{
		if (GetWorld())
		{
			FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGASAbilityBase::OnCooldownEndTimer);
			FTimerManager& TimerManager = World->GetTimerManager();

			TimerManager.SetTimer(CooldownTimerHandle, del, Cooldown, false, Cooldown);
			bIsOnCooldown = true;
		}
	}
	OnAbilityExecuted();
	bIsAbilityExecuting = true;
}

void UGASAbilityBase::OnAbilityCancelNative()
{
	OnAbilityCancel();
}

void UGASAbilityBase::FinishExecution()
{
	bIsAbilityExecuting = false;
}
void UGASAbilityBase::NativeFinishExecution()
{
	bIsAbilityExecuting = false;
}

bool UGASAbilityBase::IsWaitingForConfirm()
{
	if (OnConfirmDelegate.IsBound())
		return true;
	else
		return false;
}
void UGASAbilityBase::ConfirmAbility()
{
	if (OnConfirmDelegate.IsBound())
		OnConfirmDelegate.Broadcast();
	OnConfirmDelegate.Clear();
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
	return OwningComp.Get();
}
/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnTaskActivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Started; %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Add(&Task);
	float blabla = 0;
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

void UGASAbilityBase::OnCooldownEndTimer()
{
	bIsOnCooldown = false;
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.ClearTimer(CooldownTimerHandle);
}

bool UGASAbilityBase::IsNameStableForNetworking() const
{
	return bIsNameStable;
}

void UGASAbilityBase::SetNetAddressable()
{
	bIsNameStable = true;
}

class UWorld* UGASAbilityBase::GetWorld() const
{
	return World;
}