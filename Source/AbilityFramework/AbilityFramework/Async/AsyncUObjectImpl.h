// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "HAL/Runnable.h"
#include "AsyncUObject.h"
class FAsyncUObjectRunnable : public FRunnable
{
public:
	FAsyncUObjectRunnable()
	{

	}
	FAsyncUObjectRunnable(class UObject* InOuter)
	{
		Outer = InOuter;
	}
	/*Runs the runnable object.
		*
		* This is where all per object thread work is done.This is only called if the initialization was successful.
		*
		* @return The exit code of the runnable object
		* @see Init, Stop, Exit
		*/
	virtual uint32 Run() override
	{
		Obj = NewObject<UAsyncUObject>(Outer.Get(), UAsyncUObject::StaticClass(), TEXT("AwesomeObject"), RF_StrongRefOnFrame | RF_Standalone);
		if (Obj.IsValid())
		{
			Obj->JustTestFunction();
		}
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
private:
	TWeakObjectPtr<UAsyncUObject> Obj;
	TWeakObjectPtr<class UObject> Outer;
};
/**
 * 
 */
class ABILITYFRAMEWORK_API AsyncUObjectImpl
{
public:
	AsyncUObjectImpl();
	~AsyncUObjectImpl();
};
