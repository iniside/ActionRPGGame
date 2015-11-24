// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../Targeting/GASAbilityTargetingObject.h"
#include "../GASAbilityBase.h"
#include "GASAbilityTask_TargetData.h"


UGASAbilityTask_TargetData* UGASAbilityTask_TargetData::CreateTargetDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityTargetingObject> Class, EGASConfirmType ConfirmTypeIn)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_TargetData>(WorldContextObject);

	if (MyObj)
	{
		MyObj->ConfirmType = ConfirmTypeIn;
	}
	return MyObj;
}

void UGASAbilityTask_TargetData::Activate()
{
	switch (ConfirmType)
	{
		case EGASConfirmType::Instant:
		{
			TargetObj->GetTarget();
		}
		case EGASConfirmType::WaitForPlayer:
		{
			if (Ability.IsValid())
			{
				if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
				{
					Ability->OnConfirmDelegate.AddUObject(this, &UGASAbilityTask_TargetData::OnConfirm);
				}
			}
		}
	}
	if (TargetObj)
	{
		UE_LOG(GameAbilities, Log, TEXT("TArget object spawned"));
	}
	else
	{
		UE_LOG(GameAbilities, Log, TEXT("TArget object is null."));
	}
	//EndTask();
}

// ---------------------------------------------------------------------------------------

bool UGASAbilityTask_TargetData::BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<UGASAbilityTargetingObject> Class, UGASAbilityTargetingObject*& SpawnedActor)
{
	SpawnedActor = Class.GetDefaultObject();//NewObject<UGASAbilityTargetingObject>(WorldContextObject, Class);

	if (SpawnedActor == nullptr)
	{
		//Failure.Broadcast(nullptr);
		return false;
	}
	else
	{
		TargetObj = SpawnedActor;
		SpawnedActor->AbilityOwner = Ability;
	}
	UE_LOG(GameAbilities, Log, TEXT("Begin Spawning Actor in GASAbilityTask_SpawnActor"));
	return true;
}

void UGASAbilityTask_TargetData::FinishSpawningActor(UObject* WorldContextObject, UGASAbilityTargetingObject* SpawnedActor)
{
	if (SpawnedActor)
	{
		//Success.Broadcast(SpawnedActor);
	}
	UE_LOG(GameAbilities, Log, TEXT("Finish Spawning Actor in GASAbilityTask_SpawnActor"));
	ReadyForActivation();
}

void UGASAbilityTask_TargetData::OnConfirm()
{
	TargetObj->GetTarget();
	EndTask();
}