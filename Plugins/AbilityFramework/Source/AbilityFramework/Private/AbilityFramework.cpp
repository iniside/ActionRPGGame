// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "AbilityFramework.h"
#include "AFCueManager.h"
#include "Misc/CoreDelegates.h"
DEFINE_LOG_CATEGORY(AbilityFramework);
DEFINE_LOG_CATEGORY(GameAttributesGeneral);
DEFINE_LOG_CATEGORY(GameAttributes);
DEFINE_LOG_CATEGORY(GameAttributesEffects);

DEFINE_LOG_CATEGORY(AFAttributes);
DEFINE_LOG_CATEGORY(AFEffects);
DEFINE_LOG_CATEGORY(AFAbilities);

FAFEffectTimerManager* FAFEffectTimerManager::Instance = nullptr;

DECLARE_CYCLE_STAT(TEXT("EffectTimer.Run"), STAT_EffectTimerRun, STATGROUP_EffectTimer);
FAFEffectTimer::FAFEffectTimer()
{

}
FAFEffectTimerWorker::FAFEffectTimerWorker()
{
	bActive = true;
	Timers.Reset();
	Timers.SetNumZeroed(0);
	InternalTime = FPlatformTime::ToSeconds64(FPlatformTime::Cycles64());
	for (int Idx = 0; Idx < 10000; Idx++)
	{
		float Duration = FMath::RandRange(15, 25);
		AddTimer(Duration, 0.2);
	}
}
FAFEffectTimerManager::FAFEffectTimerManager()
{
	TimerWorker = new FAFEffectTimerWorker();
	TimerThread = FRunnableThread::Create(TimerWorker, TEXT("EffectTimerThread"), 512*1024, TPri_Normal);

	check(TimerWorker != nullptr);
}
FAFEffectTimerManager::~FAFEffectTimerManager()
{
	//if (TimerThread)
	//{
	//	TimerThread->Kill();
	//	delete TimerThread;
	//	TimerThread = nullptr;
	//}
}
uint32 FAFEffectTimerWorker::Run()
{
	while (1)
	{
		SCOPE_CYCLE_COUNTER(STAT_EffectTimerRun);
		InternalTime = FPlatformTime::ToSeconds64(FPlatformTime::Cycles64());
		//while (Timers.Num() > 0)
		{
//#if UE_BUILD_DEVELOPTMENT || UE_BUILD_DEBUG
			
//#endif // UE_BUILD_DEVELOPTMENT || UE_BUILD_DEBUG

			//for (FAFEffectTimer& Timer : Timers)
			{
				//auto It = Timers.CreateIterator();
				//for (; It; ++It)
				//{

				//	if (It->bHavePeriod && (InternalTime > It->NextPeriodTime))
				//	{
				//		double asdasd = It->ExpireTime - InternalTime;
				//		double FinalTime = InternalTime - It->StartTime;
				//		//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer Period TimeDifference: %f FinalTime: %f"), asdasd, FinalTime);

				//		It->NextPeriodTime = InternalTime + It->PeriodTime;
				//		//It->PeriodDelegate.ExecuteIfBound();

				//	}
				//	if (It->bHaveDuration && (InternalTime > It->ExpireTime))
				//	{
				//		double asdasd = It->ExpireTime - InternalTime;
				//		double FinalTime = InternalTime - It->StartTime;
				//		//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer Finish TimeDifference: %f FinalTime: %f"), asdasd, FinalTime);
				//		//It->ExpireDelegate.ExecuteIfBound();

				//		//FAFEffectTimer Out;
				//		//Timers.Remove(*It);
				//		//Timers.Shrink();
				//	}
				//}
				for (FAFEffectTimer& Timer : Timers)
				{
					if (!Timer.bActive)
						continue;

					if (Timer.bHavePeriod && (InternalTime > Timer.NextPeriodTime))
					{
						double asdasd = Timer.ExpireTime - InternalTime;
						double FinalTime = InternalTime - Timer.StartTime;
						//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer Period TimeDifference: %f FinalTime: %f"), asdasd, FinalTime);

						Timer.NextPeriodTime = InternalTime + Timer.PeriodTime;
						//Timer.PeriodDelegate.ExecuteIfBound();

					}
					if (Timer.bHaveDuration && (InternalTime > Timer.ExpireTime))
					{
						double asdasd = Timer.ExpireTime - InternalTime;
						double FinalTime = InternalTime - Timer.StartTime;
						//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer Finish TimeDifference: %f FinalTime: %f"), asdasd, FinalTime);
						//Timer.ExpireDelegate.ExecuteIfBound();
						//Timer.bActive = false;
						//FAFEffectTimer Out;
						//Timers.Remove(*It);
						//Timers.Shrink();
					}
				}
				//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer TimeRemaining: %f"), asdasd);
				//FAFEffectTimer& Top = Timers.HeapTop();
				
				//if (Timer.bActive)
				//{
				//	if (Top.bHaveDuration && (InternalTime > Top.ExpireTime))
				//	{
				//		double asdasd = Top.ExpireTime - InternalTime;
				//		double FinalTime = InternalTime - Top.StartTime;
				//		//UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer Finish TimeDifference: %f FinalTime: %f"), asdasd, FinalTime);
				//		Top.ExpireDelegate.ExecuteIfBound();

				//		FAFEffectTimer Out;
				//		Timers.HeapPop(Out);
				//		Timers.Shrink();
				//	}
				//}
			}
		}
		FPlatformProcess::Sleep(0.01);
	}
	return 0;
}
bool FAFEffectTimerWorker::Init()
{
	return true;
}
void FAFEffectTimerWorker::Stop()
{ 
	bActive = false;
}

/**
* Exits the runnable object.
*
* Called in the context of the aggregating thread to perform any cleanup.
* @see Init, Run, Stop
*/
void FAFEffectTimerWorker::Exit()
{
	//Timers.Empty();
	//bActive = false;
}

FAFEffectTimeHandle FAFEffectTimerWorker::AddTimer(double InDuration, double InPeriod)
{
	FScopeLock lock(&CS);
	//bActive = false;
	FAFEffectTimer Timer;
	Timer.Duration = InDuration;
	Timer.PeriodTime = InPeriod;
	Timer.bHaveDuration = InDuration <= 0 ? false : true;
	Timer.bHavePeriod = InPeriod <= 0 ? false : true;
	Timer.StartTime = InternalTime; // FPlatformTime::ToSeconds64(FPlatformTime::Cycles64());// InternalTime;

	Timer.ExpireTime = InternalTime + Timer.Duration;
	Timer.NextPeriodTime = InternalTime + Timer.PeriodTime;
	Timer.bActive = true;
	Timers.Add(Timer);
	int32 idx = Timers.Num() - 1;
	//Timers.Push(Timer);
	//Timers.Heapify();
	FAFEffectTimeHandle Handle;
	Handle.Index = idx;
	Timers[idx].Handle = Handle;
	UE_LOG(GameAttributesEffects, Log, TEXT("AsyncTimer  Start TimeDifference: %f"), (Timer.ExpireTime - InternalTime));

	//since there is at least one timer, set timer manager to be active.
	
	bActive = true;
	return Handle;
}

FAFEffectTimeHandle FAFEffectTimerManager::AddTimer(double InDuration, double InPeriod)
{

	return TimerWorker->AddTimer(InDuration, InPeriod);
}

void FAbilityFramework::StartupModule()
{
#if WITH_EDITOR
	//FModuleManager::Get().LoadModule(TEXT("AbilityFrameworkEditor"));
#endif //WITH_EDITOR
	//FAFEffectTimerManager::Get();
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)

	//initialize existing cues.
	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FAbilityFramework::InitCues);
}


void FAbilityFramework::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FAbilityFramework::InitCues()
{
	UAFCueManager::Get();
}

IMPLEMENT_MODULE(FAbilityFramework, AbilityFramework)