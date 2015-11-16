// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilityTask_WaitForConfirm.h"




UGASAbilityTask_WaitForConfirm* UGASAbilityTask_WaitForConfirm::CreateTargetDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_WaitForConfirm>(WorldContextObject);
	//if(MyObj)
	//	MyObj->SpawnObject(WorldContextObject, InClass, Outer);
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}

void UGASAbilityTask_WaitForConfirm::Initialize()
{
	if (Ability.IsValid())
	{
		Ability.ConfirmDelegate.CreateUObject(this, &UGASAbilityTask_WaitForConfirm::OnConfirm);
	}
}

void UGASAbilityTask_WaitForConfirm::OnConfirm()
{
	OnConfirmed.Broadcast();
	EndTask();
}