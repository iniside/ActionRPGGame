// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "../GAAbilitiesComponent.h"
#include "GAEffectExtension.h"

UGAEffectExtension::UGAEffectExtension(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}
void UGAEffectExtension::SetParameters(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}
void UGAEffectExtension::BeginEffect()
{
}

void UGAEffectExtension::NativeOnEffectApplied()
{
	OnEffectApplied();
}
void UGAEffectExtension::NativeOnEffectExecuted()
{
	OnEffectExecuted();
}
void UGAEffectExtension::NativeOnEffectExpired()
{
	OnEffectExpired();
}
void UGAEffectExtension::NativeOnEffectRemoved()
{
	OnEffectRemoved();
}
/** GameplayTaskOwnerInterface - Begin */
void UGAEffectExtension::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	//if (UGAAbilityTask* task = Cast<UGAAbilityTask>(&Task))
	//{
	//	task->Ability = this;
	//}
}
UGameplayTasksComponent* UGAEffectExtension::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return OwningComponent;
}
/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
void UGAEffectExtension::OnGameplayTaskActivated(UGameplayTask& Task)
{
	UE_LOG(AbilityFramework, Log, TEXT("Task Started; %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Add(&Task);
	//AbilityComponent->OnGameplayTaskActivated(Task);
}
/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
void UGAEffectExtension::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(AbilityFramework, Log, TEXT("Task Removed: %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Remove(&Task);
	//AbilityComponent->OnGameplayTaskDeactivated(Task);
}
AActor* UGAEffectExtension::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return OwningComponent->GetOwner();
}
AActor* UGAEffectExtension::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return Avatar;
}
/** GameplayTaskOwnerInterface - end */

UWorld* UGAEffectExtension::GetWorld() const
{
	if (Context.Target.IsValid())
		return Context.Target->GetWorld();

	return nullptr;
}