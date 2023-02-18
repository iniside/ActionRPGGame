// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine.h"
#include "IAbilityFramework.h"
#include "InputCoreTypes.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
//#include "GameTrace.h"

enum class EAFEffectTimerStatus : uint8
{
	Pending,
	Active,
	Executing,
	Paused
};

struct FAFEffectTimeHandle
{
	int32 Index;
	bool operator==(const FAFEffectTimeHandle& Other) const
	{
		return Index == Other.Index;
	}
};

struct FAFEffectTimer
{
	double StartTime; //when started
	double ExpireTime;
	double NextPeriodTime;
	double PeriodTime;
	double Duration; //how often timer is executed

	bool bHaveDuration; //does it loop
	bool bHavePeriod;
	bool bActive;

	EAFEffectTimerStatus Status;

	FSimpleDelegate ExpireDelegate;
	FSimpleDelegate PeriodDelegate;

	FAFEffectTimeHandle Handle;

	FAFEffectTimer();

	bool operator<(const FAFEffectTimer& Other) const
	{
		return ExpireTime < Other.ExpireTime;
	}
	bool operator==(const FAFEffectTimer& Other) const
	{
		return Handle == Other.Handle;
	}
};

DECLARE_STATS_GROUP(TEXT("EffectTimer"), STATGROUP_EffectTimer, STATCAT_Advanced);
class FAFEffectTimerWorker : public FRunnable
{
	FCriticalSection CS;
	TArray<FAFEffectTimer> Timers;
	TArray<FAFEffectTimer> ActiveTimers;
	UWorld* World;
	double InternalTime;
	bool bActive;
public:
	FAFEffectTimerWorker();
	virtual bool Init() override;

	/**
	* Runs the runnable object.
	*
	* This is where all per object thread work is done. This is only called if the initialization was successful.
	*
	* @return The exit code of the runnable object
	* @see Init, Stop, Exit
	*/
	virtual uint32 Run() override;

	/**
	* Stops the runnable object.
	*
	* This is called if a thread is requested to terminate early.
	* @see Init, Run, Exit
	*/
	virtual void Stop() override;

	/**
	* Exits the runnable object.
	*
	* Called in the context of the aggregating thread to perform any cleanup.
	* @see Init, Run, Stop
	*/
	virtual void Exit() override;

	FAFEffectTimeHandle AddTimer(double InDuration, double InPeriod);
};

class FAFEffectTimerManager
{
	FRunnableThread* TimerThread;
	FAFEffectTimerWorker* TimerWorker;
	

public:
	static FAFEffectTimerManager* Instance;
	static FAFEffectTimerManager* Get()
	{
		//static FAFEffectTimerManager Obj;// = nullptr;
		//return Obj;
		if (!Instance)
			Instance = new FAFEffectTimerManager();

		return Instance;

	}

	FAFEffectTimerManager();

	~FAFEffectTimerManager();

	void TickTimer(float InDeltaTime);
	FAFEffectTimeHandle AddTimer(double InDuration, double InPeriod);
};

class FAbilityFramework : public IAbilityFramework
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void InitCues();
};


DECLARE_LOG_CATEGORY_EXTERN(AbilityFramework, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(GameAttributesGeneral, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(GameAttributes, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(GameAttributesEffects, Log, All);



DECLARE_LOG_CATEGORY_EXTERN(AFAttributes, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(AFEffects, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(AFAbilities, Log, All);