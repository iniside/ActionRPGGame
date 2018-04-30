// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask_TargetDataCircle.h"


UGAAbilityTask_TargetDataCircle* UGAAbilityTask_TargetDataCircle::TargetCircleDataTask(UGAAbilityBase* WorldContextObject,
	FName InTaskName, EGASConfirmType ConfirmTypeIn)
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
	//EndTask();
}

// ---------------------------------------------------------------------------------------

//bool UGAAbilityTask_TargetDataCircle::BeginSpawningActor(UObject* WorldContextObject, UGASAbilityTargetingObject*& SpawnedActor)
//{
//	SpawnedActor = Class.GetDefaultObject();//NewObject<UGASAbilityTargetingObject>(WorldContext
//	return true;
//}
//
//void UGAAbilityTask_TargetDataCircle::FinishSpawningActor(UObject* WorldContextObject, UGASAbilityTargetingObject* SpawnedActor)
//{
//	if (SpawnedActor)
//	{
//		//Success.Broadcast(SpawnedActor);
//	}
//	UE_LOG(AbilityFramework, Log, TEXT("Finish Spawning Actor in GAAbilityTask_SpawnActor"));
//	ReadyForActivation();
//}

void UGAAbilityTask_TargetDataCircle::OnConfirm()
{
	//TargetObj2->GetTarget();
	EndTask();
}