// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "GAAbilityTask_SpawnActor.h"





UGAAbilityTask_SpawnActor* UGAAbilityTask_SpawnActor::SpawnActor(UGAAbilityBase* WorldContextObject,
	FName InTaskName, TSubclassOf<AActor> InClass)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_SpawnActor>(WorldContextObject);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

// ---------------------------------------------------------------------------------------
void UGAAbilityTask_SpawnActor::Activate()
{
	UE_LOG(AbilityFramework, Log, TEXT("TArget object spawned"));
	//EndTask();
}

bool UGAAbilityTask_SpawnActor::BeginSpawningActor(UGAAbilityBase* WorldContextObject, TSubclassOf<AActor> InClass, AActor*& SpawnedActor)
{
	//if (Ability.IsValid() && Ability.Get()->GetCurrentActorInfo()->IsNetAuthority())
	//{
		UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
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
	UE_LOG(AbilityFramework, Log, TEXT("Begin Spawning Actor in GAAbilityTask_SpawnActor"));
	return true;
}

void UGAAbilityTask_SpawnActor::FinishSpawningActor(UGAAbilityBase* WorldContextObject, AActor* SpawnedActor)
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
	UE_LOG(AbilityFramework, Log, TEXT("Finish Spawning Actor in GAAbilityTask_SpawnActor"));
	//EndTask();
}
