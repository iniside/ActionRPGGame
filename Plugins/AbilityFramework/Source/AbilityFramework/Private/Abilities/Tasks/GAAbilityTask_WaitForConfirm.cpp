// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask_WaitForConfirm.h"




UGAAbilityTask_WaitForConfirm* UGAAbilityTask_WaitForConfirm::CreateWaitConfirmTask(UGAAbilityBase* WorldContextObject,
	FName InTaskName)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_WaitForConfirm>(WorldContextObject, InTaskName);
	return MyObj;
}
void UGAAbilityTask_WaitForConfirm::Activate()
{
	if (AbilityComponent.IsValid() && Ability.IsValid())
	{
		GetOuterUGAAbilityBase()->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_WaitForConfirm::OnConfirm);
	}
}
void UGAAbilityTask_WaitForConfirm::Initialize()
{

}

void UGAAbilityTask_WaitForConfirm::OnConfirm()
{
	Ability->OnConfirmDelegate.Clear();
	OnConfirmed.Broadcast();
	EndTask();
}