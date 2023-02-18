// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFLatentInterface.h"

#include "AFTaskBase.h"

void FGALatentFunctionTick::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Target && !Target->IsPendingKillOrUnreachable())
	{
		FScopeCycleCounterUObject ActorScope(Target);
		Target->TickTask(DeltaTime, TickType, *this);
	}
}

FString FGALatentFunctionTick::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[TickAction]");
}

UAFTaskBase::UAFTaskBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	TickFunction.TickGroup = TG_PrePhysics;
	// Default to no tick function, but if we set 'never ticks' to false (so there is a tick function) it is enabled by default
	TickFunction.bCanEverTick = false;
	TickFunction.bStartWithTickEnabled = true;
	TickFunction.bAllowTickOnDedicatedServer = true;
	TickFunction.bRunOnAnyThread = true;

	TickFunction.SetTickFunctionEnable(true);
	SetFlags(RF_StrongRefOnFrame);
	TaskState = EState::Waiting;
}

void UAFTaskBase::Initialize()
{
	if (GetWorld())
	{
		TickFunction.Target = this;
		ULevel* level = GetWorld()->GetCurrentLevel();
		if (level)
		{
			TickFunction.RegisterTickFunction(level);
		}
	}
}

void UAFTaskBase::ReadyForActivation()
{
	if (TaskOwner)
	{
		if (TaskState != EState::Active)
		{
			TaskState = EState::Active;
			Activate();
			Cast<IAFLatentInterface>(TaskOwner)->OnLatentTaskActivated(this);
		}
		
	}
	else
	{
		EndTask();
	}
}

void UAFTaskBase::EndTask()
{
	if (TickFunction.bCanEverTick && TickFunction.IsTickFunctionRegistered())
	{
		TickFunction.UnRegisterTickFunction();
		TickFunction.SetTickFunctionEnable(false);
	}
	OnTaskEnded();
	Cast<IAFLatentInterface>(TaskOwner)->OnLatentTaskDeactivated(this);
	TaskState = EState::Finished;
	//MarkPendingKill();
}
void UAFTaskBase::BeginDestroy()
{
	Super::BeginDestroy();
}

bool UAFTaskBase::IsNameStableForNetworking() const
{
	return false;
}
void UAFTaskBase::SetNetAddressable()
{

}
UWorld* UAFTaskBase::GetWorld() const
{
	if (TaskOwner)
	{
		return TaskOwner->GetWorld();
	}
	return nullptr;
}