// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "Entity/GASAbilityMod.h"
#include "GASAbilityTask_CreateObject.h"

UGASAbilityTask_CreateObject* UGASAbilityTask_CreateObject::CreateObject(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_CreateObject>(WorldContextObject);
	//if(MyObj)
	//	MyObj->SpawnObject(WorldContextObject, InClass, Outer);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

// ---------------------------------------------------------------------------------------

bool UGASAbilityTask_CreateObject::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<UGASAbilityMod> Class, UGASAbilityMod*& SpawnedActor)
{
	//if (Ability.IsValid() && Ability.Get()->GetCurrentActorInfo()->IsNetAuthority())
	//{
	//UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
	//if (World)
	//{
		SpawnedActor = NewObject<UGASAbilityMod>(WorldContextObject, Class);
	//}
	//}

	if (SpawnedActor == nullptr)
	{
		Failure.Broadcast(nullptr);
		return false;
	}
	UE_LOG(GameAbilities, Log, TEXT("Begin Spawning Actor in GASAbilityTask_SpawnActor"));
	return true;
}

void UGASAbilityTask_CreateObject::FinishSpawningActor(UObject* WorldContextObject, UGASAbilityMod* SpawnedActor)
{
	if (SpawnedActor)
	{
		FTransform SpawnTransform;// = AbilitySystemComponent->GetOwner()->GetTransform();

								  //if (FGameplayAbilityTargetData* LocationData = CachedTargetDataHandle.Get(0))		//Hardcode to use data 0. It's OK if data isn't useful/valid.
								  //{
								  //	//Set location. Rotation is unaffected.
								  //	if (LocationData->HasHitResult())
								  //	{
								  //		SpawnTransform.SetLocation(LocationData->GetHitResult()->Location);
								  //	}
								  //	else if (LocationData->HasEndPoint())
								  //	{
								  //		SpawnTransform.SetLocation(LocationData->GetEndPoint());
								  //	}
								  //}

		//SpawnedActor->FinishSpawning(SpawnTransform);

		Success.Broadcast(SpawnedActor);
	}
	UE_LOG(GameAbilities, Log, TEXT("Finish Spawning Actor in GASAbilityTask_SpawnActor"));
	EndTask();
}
