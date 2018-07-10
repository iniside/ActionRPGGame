// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFEffectsComponent.h"
#include "Effects/EffectTasks/AFEffectTask.h"
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
	CleanupTasks();
}
void UGAEffectExtension::NativeOnEffectRemoved()
{
	OnEffectRemoved();
	CleanupTasks();
}

void UGAEffectExtension::NativeOnEffectAppliedCDO()
{
	OnEffectAppliedCDO();
}
void UGAEffectExtension::NativeOnEffectExecutedCDO()
{
	OnEffectExecutedCOD();
}
void UGAEffectExtension::NativeOnEffectExpiredCDO()
{
	OnEffectExpiredCDO();
}
void UGAEffectExtension::NativeOnEffectRemovedCDO()
{
	OnEffectRemovedCDO();
}

UWorld* UGAEffectExtension::GetWorld() const
{
	if (Context.Target.IsValid())
		return Context.Target->GetWorld();

	return nullptr;
}

void UGAEffectExtension::OnLatentTaskAdded(FName InstanceName, class UAFTaskBase* TaskIn)
{
	ActiveTasks.Add(TaskIn);
	if (!InstanceName.IsNone())
	{
		Tasks.Add(InstanceName, TaskIn);
	}
};
void UGAEffectExtension::AddReplicatedTask(class UAFTaskBase* TaskIn)
{
	//AbilityComponent->ReplicatedTasks.Add(TaskIn);
}
void UGAEffectExtension::OnLatentTaskRemoved(class UAFTaskBase* TaskIn)
{
};

void UGAEffectExtension::OnLatentTaskActivated(class UAFTaskBase* TaskIn)
{
};
void UGAEffectExtension::OnLatentTaskDeactivated(class UAFTaskBase* TaskIn)
{
};

class UAFTaskBase* UGAEffectExtension::GetCachedLatentAction(FName TaskName)
{
	return Tasks.FindRef(TaskName);
}

void UGAEffectExtension::CleanupTasks()
{
	for (UAFTaskBase* Task : ActiveTasks)
	{
		Task->EndTask();
	}

	ActiveTasks.Reset();
	ActiveTasks.Shrink();

	Tasks.Reset();
	Tasks.Shrink();
}