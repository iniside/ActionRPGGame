// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "../../Targeting/GASAbilityTargetingObject.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask_TargetDataCircle.h"


UGAAbilityTask_TargetDataCircle* UGAAbilityTask_TargetDataCircle::TargetCircleDataTask(UObject* WorldContextObject, 
	FName InTaskName, TSubclassOf<class UGASAbilityTargetingObject> Class, EGASConfirmType ConfirmTypeIn)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_TargetDataCircle>(WorldContextObject);

	if (MyObj)
	{
		MyObj->ConfirmType = ConfirmTypeIn;
	}
	return MyObj;
}

void UGAAbilityTask_TargetDataCircle::Activate()
{
	switch (ConfirmType)
	{
		case EGASConfirmType::Instant:
		{
			TargetObj2->GetTarget();
		}
		case EGASConfirmType::WaitForConfirm:
		{
			if (Ability.IsValid())
			{
				if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_TargetDataCircle::OnConfirm);
				}
			}
		}
	}
	if (TargetObj2)
	{
		UE_LOG(AbilityFramework, Log, TEXT("TArget object spawned"));
	}
	else
	{
		UE_LOG(AbilityFramework, Log, TEXT("TArget object is null."));
	}
	//EndTask();
}

// ---------------------------------------------------------------------------------------

bool UGAAbilityTask_TargetDataCircle::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<UGASAbilityTargetingObject> Class, UGASAbilityTargetingObject*& SpawnedActor)
{
	SpawnedActor = Class.GetDefaultObject();//NewObject<UGASAbilityTargetingObject>(WorldContextObject, Class);

	if (SpawnedActor == nullptr)
	{
		//Failure.Broadcast(nullptr);
		return false;
	}
	else
	{
		TargetObj2 = SpawnedActor;
		SpawnedActor->AbilityOwner = Ability;
	}
	UE_LOG(AbilityFramework, Log, TEXT("Begin Spawning Actor in GAAbilityTask_SpawnActor"));
	return true;
}

void UGAAbilityTask_TargetDataCircle::FinishSpawningActor(UObject* WorldContextObject, UGASAbilityTargetingObject* SpawnedActor)
{
	if (SpawnedActor)
	{
		//Success.Broadcast(SpawnedActor);
	}
	UE_LOG(AbilityFramework, Log, TEXT("Finish Spawning Actor in GAAbilityTask_SpawnActor"));
	ReadyForActivation();
}

void UGAAbilityTask_TargetDataCircle::OnConfirm()
{
	TargetObj2->GetTarget();
	EndTask();
}