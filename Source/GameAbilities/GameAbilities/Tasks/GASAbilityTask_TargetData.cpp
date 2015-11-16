// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilityTask_TargetData.h"


UGASAbilityTask_TargetData* UGASAbilityTask_TargetData::CreateTargetDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_TargetData>(WorldContextObject);
	//if(MyObj)
	//	MyObj->SpawnObject(WorldContextObject, InClass, Outer);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

// ---------------------------------------------------------------------------------------

bool UGASAbilityTask_TargetData::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<UGASAbilityMod> Class, UGASAbilityMod*& SpawnedActor)
{
	SpawnedActor = NewObject<UGASAbilityMod>(WorldContextObject, Class);

	if (SpawnedActor == nullptr)
	{
		//Failure.Broadcast(nullptr);
		return false;
	}
	UE_LOG(GameAbilities, Log, TEXT("Begin Spawning Actor in GASAbilityTask_SpawnActor"));
	return true;
}

void UGASAbilityTask_TargetData::FinishSpawningActor(UObject* WorldContextObject, UGASAbilityMod* SpawnedActor)
{
	if (SpawnedActor)
	{
		//Success.Broadcast(SpawnedActor);
	}
	UE_LOG(GameAbilities, Log, TEXT("Finish Spawning Actor in GASAbilityTask_SpawnActor"));
	EndTask();
}


