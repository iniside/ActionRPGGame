// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "AsyncUObject.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAsyncUObject : public UObject, public FRunnable
{
	GENERATED_BODY()
	
public:
	void JustTestFunction();
	
	virtual uint32 Run() override
	{
	/*	Obj = NewObject<UAsyncUObject>(Outer.Get(), UAsyncUObject::StaticClass(), TEXT("AwesomeObject"), RF_StrongRefOnFrame | RF_Standalone);
		if (Obj.IsValid())
		{
			Obj->JustTestFunction();
		}*/
		JustTestFunction();
		float somerandomcrap = 0;
		return 0;
	}

	/**
	* Stops the runnable object.
	*
	* This is called if a thread is requested to terminate early.
	* @see Init, Run, Exit
	*/
	virtual void Stop() override { }

	/**
	* Exits the runnable object.
	*
	* Called in the context of the aggregating thread to perform any cleanup.
	* @see Init, Run, Stop
	*/
	virtual void Exit() override { }
};
