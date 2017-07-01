// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GALatentFunctionBase.h"

void FGALatentFunctionTick::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Target && !Target->IsPendingKillOrUnreachable())
	{
		FScopeCycleCounterUObject ActorScope(Target);
		Target->TickAction(DeltaTime, TickType, *this);
	}
}

FString FGALatentFunctionTick::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[TickAction]");
}

UGALatentFunctionBase::UGALatentFunctionBase(const FObjectInitializer& ObjectInitializer)
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
}

void UGALatentFunctionBase::Initialize()
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
void UGALatentFunctionBase::EndTask()
{
	if (TickFunction.bCanEverTick && TickFunction.IsTickFunctionRegistered())
	{
		TickFunction.UnRegisterTickFunction();
		TickFunction.SetTickFunctionEnable(false);
	}
	MarkPendingKill();
}
void UGALatentFunctionBase::BeginDestroy()
{
	Super::BeginDestroy();
	if (Endpoint.IsValid())
	{
		FMessageEndpoint::SafeRelease(Endpoint);
		Endpoint.Reset();
	}
}
UWorld* UGALatentFunctionBase::GetWorld() const
{
	if (TaskOwner)
	{
		return TaskOwner->GetWorld();
	}
	return nullptr;
}