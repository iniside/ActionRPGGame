// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#include "GameAbilities.h"
#include "Tasks/GASAbilityTask.h"
#include "States/GASAbilityStateActive.h"
#include "States/GASAbilityStateCasting.h"
#include "States/GASAbilityStateCooldown.h"
#include "GASAbilityBase.h"

UGASAbilityBase::UGASAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAbilityExecuting = false;
	bIsOnCooldown = false;
	bReplicate = true;
	bIsNameStable = false;

	DefaultState = ObjectInitializer.CreateDefaultSubobject<UGASAbilityStateActive>(this, TEXT("AbilityDefaultState"));
	//ActivationState = ObjectInitializer.CreateDefaultSubobject<UGASAbilityStateCasting>(this, TEXT("AbilityActivationState"));
	//CurrentState = ObjectInitializer.CreateDefaultSubobject<UGASAbilityStateCooldown>(this, TEXT("AbilityCooldownState"));
}

void UGASAbilityBase::InitAbility()
{
	if (OwningComp.IsValid())
	{
		World = OwningComp->GetWorld();
	}
	CurrentState = DefaultState;
}

void UGASAbilityBase::OnNativeInputPressed()
{
	UE_LOG(GameAbilities, Log, TEXT("OnNativeInputPressed in ability %s"), *GetName());
	OnInputPressed();
}

void UGASAbilityBase::OnNativeInputReleased()
{
	UE_LOG(GameAbilities, Log, TEXT("OnNativeInputReleased in ability %s"), *GetName());
	OnInputReleased();
}

void UGASAbilityBase::OnAbilityExecutedNative()
{
	UE_LOG(GameAbilities, Log, TEXT("Begin Executing Ability: %s"), *GetName());
	OnAbilityExecuted();
	bIsAbilityExecuting = true;
}

void UGASAbilityBase::ExecuteAbility()
{
	if (CurrentState)
		CurrentState->BeginActionSequence();
	//OnAbilityExecutedNative();
}

void UGASAbilityBase::StopAbility()
{
	if (CurrentState)
		CurrentState->EndActionSequence();
	//OnAbilityExecutedNative();
}

void UGASAbilityBase::OnNativeStopAbility()
{
	OnStopAbility();
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

bool UGASAbilityBase::CanUseAbility()
{
	bool CanUse = true;
	CanUse = !bIsOnCooldown;
	return CanUse;

}

void UGASAbilityBase::GotoState(class UGASAbilityState* NextState)
{
	if (NextState == NULL || !NextState->IsIn(this))
	{

	}
	else if (CurrentState != NextState)
	{
		UGASAbilityState* PrevState = CurrentState;
		if (CurrentState != NULL)
		{
			CurrentState->EndState();
		}
		if (CurrentState == PrevState)
		{
			CurrentState = NextState;
			CurrentState->BeginState(PrevState);
		}
	}
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
}
/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Removed: %s in ability: %s"), *Task.GetName(), *GetName());
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