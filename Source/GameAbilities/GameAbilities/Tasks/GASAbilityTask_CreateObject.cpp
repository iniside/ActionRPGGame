// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "Entity/GASAbilityMod.h"
#include "GASAbilityTask_CreateObject.h"

UGASAbilityTask_CreateObject* UGASAbilityTask_CreateObject::CreateObject(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> InClass, UObject* Outer)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_CreateObject>(WorldContextObject);
	//if(MyObj)
	//	MyObj->SpawnObject(InClass, Outer);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

// ---------------------------------------------------------------------------------------

class UGASAbilityMod* UGASAbilityTask_CreateObject::SpawnObject(TSubclassOf<class UGASAbilityMod> InClass, UObject* Outer)
{
	UGASAbilityMod* SpawnedObject = NewObject<UGASAbilityMod>(Outer, InClass);
	if (SpawnedObject == nullptr)
	{
		Failure.Broadcast(nullptr);
		return false;
	}
	UE_LOG(GameAbilities, Log, TEXT("Begin Spawning Actor in GASAbilityTask_SpawnActor"));
	Success.Broadcast(SpawnedObject);
	EndTask();
	return SpawnedObject;
}