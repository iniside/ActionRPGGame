// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "GASAbilityTask_WaitForConfirm.h"




UGASAbilityTask_WaitForConfirm* UGASAbilityTask_WaitForConfirm::CreateWaitConfirmTask(UObject* WorldContextObject)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_WaitForConfirm>(WorldContextObject);
	UGASAbilityBase* ability = Cast<UGASAbilityBase>(WorldContextObject);
	//if (ability)
	//{
	//	ability->ActiveTasks.Add(MyObj);
	//}
	//MyObj->CachedTargetDataHandle = TargetData;
	return MyObj;
}
void UGASAbilityTask_WaitForConfirm::Activate()
{
	if (Ability.IsValid())
	{
		if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
			Ability->OnConfirmDelegate.AddUObject(this, &UGASAbilityTask_WaitForConfirm::OnConfirm);
		//Ability->ConfirmDelegate.CreateUObject(this, &UGASAbilityTask_WaitForConfirm::OnConfirm);
	}
}
void UGASAbilityTask_WaitForConfirm::Initialize()
{

}

void UGASAbilityTask_WaitForConfirm::OnConfirm()
{
	OnConfirmed.Broadcast();
	EndTask();
}