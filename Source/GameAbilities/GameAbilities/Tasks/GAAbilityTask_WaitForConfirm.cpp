// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../GAAbilityBase.h"
#include "GAAbilityTask_WaitForConfirm.h"




UGAAbilityTask_WaitForConfirm* UGAAbilityTask_WaitForConfirm::CreateWaitConfirmTask(UObject* WorldContextObject)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_WaitForConfirm>(WorldContextObject);
	return MyObj;
}
void UGAAbilityTask_WaitForConfirm::Activate()
{
	if (Ability.IsValid())
	{
		if (!Ability->OnConfirmDelegate.IsBoundToObject(this))
			Ability->OnConfirmDelegate.AddUObject(this, &UGAAbilityTask_WaitForConfirm::OnConfirm);
		//Ability->ConfirmDelegate.CreateUObject(this, &UGAAbilityTask_WaitForConfirm::OnConfirm);
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