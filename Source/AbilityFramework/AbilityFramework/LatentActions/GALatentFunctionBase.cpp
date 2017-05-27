// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GALatentFunctionBase.h"
FString ThreadEnumToString(ENamedThreads::Type Val)
{
	FString ThreadName = "";
	switch (Val)
	{
	case ENamedThreads::UnusedAnchor:
		ThreadName = "UnusedAnchor";
		break;
	case ENamedThreads::StatsThread:
		ThreadName = "StatsThread";
		break;
	case ENamedThreads::RHIThread:
		ThreadName = "RHIThread";
		break;
	case ENamedThreads::AudioThread:
		ThreadName = "AudioThread";
		break;
	case ENamedThreads::GameThread:
		ThreadName = "GameThread";
		break;
	case ENamedThreads::ActualRenderingThread:
		ThreadName = "ActualRenderingThread";
		break;
	case ENamedThreads::AnyThread:
		ThreadName = "AnyThread";
		break;
	//case ENamedThreads::MainQueue:
	//	ThreadName = "MainQueue";
	//	break;
	case ENamedThreads::LocalQueue:
		ThreadName = "LocalQueue";
		break;
	//case ENamedThreads::NumQueues:
	//	ThreadName = "NumQueues";
	//	break;
	//case ENamedThreads::ThreadIndexMask:
	//	ThreadName = "ThreadIndexMask";
	//	break;
	//case ENamedThreads::QueueIndexMask:
	//	ThreadName = "QueueIndexMask";
	//	break;
	case ENamedThreads::QueueIndexShift:
		ThreadName = "QueueIndexShift";
		break;
	//case ENamedThreads::NormalTaskPriority:
	//	ThreadName = "NormalTaskPriority";
	//	break;
	case ENamedThreads::HighTaskPriority:
		ThreadName = "HighTaskPriority";
		break;
	//case ENamedThreads::NumTaskPriorities:
	//	ThreadName = "NumTaskPriorities";
	//	break;
	//case ENamedThreads::TaskPriorityMask:
	//	ThreadName = "TaskPriorityMask";
	//	break;
	case ENamedThreads::TaskPriorityShift:
		ThreadName = "TaskPriorityShift";
		break;
	//case ENamedThreads::NormalThreadPriority:
	//	ThreadName = "NormalThreadPriority";
	//	break;
	case ENamedThreads::HighThreadPriority:
		ThreadName = "HighThreadPriority";
		break;
	case ENamedThreads::BackgroundThreadPriority:
		ThreadName = "BackgroundThreadPriority";
		break;
	//case ENamedThreads::NumThreadPriorities:
	//	ThreadName = "NumThreadPriorities";
	//	break;
	case ENamedThreads::ThreadPriorityMask:
		ThreadName = "ThreadPriorityMask";
		break;
	case ENamedThreads::ThreadPriorityShift:
		ThreadName = "ThreadPriorityShift";
		break;
	//case ENamedThreads::StatsThread_Local:
	//	ThreadName = "StatsThread_Local";
	//	break;
	case ENamedThreads::GameThread_Local:
		ThreadName = "GameThread_Local";
		break;
	case ENamedThreads::ActualRenderingThread_Local:
		ThreadName = "ActualRenderingThread_Local";
		break;
	case ENamedThreads::AnyHiPriThreadNormalTask:
		ThreadName = "AnyHiPriThreadNormalTask";
		break;
	case ENamedThreads::AnyHiPriThreadHiPriTask:
		ThreadName = "AnyHiPriThreadHiPriTask";
		break;
	//case ENamedThreads::AnyNormalThreadNormalTask:
	//	ThreadName = "AnyNormalThreadNormalTask";
	//	break;
	case ENamedThreads::AnyNormalThreadHiPriTask:
		ThreadName = "AnyNormalThreadHiPriTask";
		break;
	case ENamedThreads::AnyBackgroundThreadNormalTask:
		ThreadName = "AnyBackgroundThreadNormalTask";
		break;
	case ENamedThreads::AnyBackgroundHiPriTask:
		ThreadName = "AnyBackgroundHiPriTask";
		break;
	default:
		break;
	}
	return ThreadName;
}
void FGALatentFunctionTick::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	FString debug = ThreadEnumToString(CurrentThread);
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