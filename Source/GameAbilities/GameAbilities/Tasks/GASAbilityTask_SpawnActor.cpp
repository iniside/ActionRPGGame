// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilityTask_SpawnActor.h"





UGASAbilityTask_SpawnActor* UGASAbilityTask_SpawnActor::SpawnActor(UObject* WorldContextObject, TSubclassOf<AActor> InClass)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_SpawnActor>(WorldContextObject);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

// ---------------------------------------------------------------------------------------
void UGASAbilityTask_SpawnActor::Activate()
{
	UE_LOG(GameAbilities, Log, TEXT("TArget object spawned"));
	//EndTask();
}

bool UGASAbilityTask_SpawnActor::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<AActor> InClass, AActor*& SpawnedActor)
{
	//if (Ability.IsValid() && Ability.Get()->GetCurrentActorInfo()->IsNetAuthority())
	//{
		UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
		if (World)
		{
			SpawnedActor = World->SpawnActorDeferred<AActor>(InClass, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}
	//}

	if (SpawnedActor == nullptr)
	{
		Failure.Broadcast(nullptr);
		return false;
	}
	UE_LOG(GameAbilities, Log, TEXT("Begin Spawning Actor in GASAbilityTask_SpawnActor"));
	return true;
}

void UGASAbilityTask_SpawnActor::FinishSpawningActor(UObject* WorldContextObject, AActor* SpawnedActor)
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

		SpawnedActor->FinishSpawning(SpawnTransform);

		Success.Broadcast(SpawnedActor);
	}
	UE_LOG(GameAbilities, Log, TEXT("Finish Spawning Actor in GASAbilityTask_SpawnActor"));
	//EndTask();
}
