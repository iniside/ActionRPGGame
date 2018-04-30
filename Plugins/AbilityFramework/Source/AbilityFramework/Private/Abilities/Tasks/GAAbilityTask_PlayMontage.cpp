// Fill out your copyright notice in the Description page of Project Settings.

#include "../../AbilityFramework.h"
#include "../../AFAbilityComponent.h"
#include "GAAbilityTask_PlayMontage.h"

UGAAbilityTask_PlayMontage* UGAAbilityTask_PlayMontage::AbilityPlayMontage(UGAAbilityBase* WorldContextObject,
	FName InTaskName, UAnimMontage* MontageIn, FName SectionNameIn, float PlayRateIn,
	bool bInUseActivationTime)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_PlayMontage>(WorldContextObject);
	MyObj->Montage = MontageIn;
	MyObj->SectionName = SectionNameIn;
	MyObj->PlayRate = PlayRateIn;
	MyObj->bUseActivationTime = bInUseActivationTime;
	return MyObj;
}

void UGAAbilityTask_PlayMontage::Activate()
{
	AbilityComponent->OnAbilityNotifyBegin.Unbind();
	AbilityComponent->OnAbilityNotifyTick.Unbind();
	AbilityComponent->OnAbilityNotifyEnd.Unbind();
	
	AbilityComponent->OnAbilityNotifyBegin.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastStartNotifyState);
	AbilityComponent->OnAbilityNotifyTick.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastTickNotifyState);
	AbilityComponent->OnAbilityNotifyEnd.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastEndNotifyState);
	if (bUseActivationTime)
	{
		PlayRate = Ability->CalculateAnimationSpeed(Montage);
		Ability->PlayMontage(Montage, SectionName, PlayRate);
	}
	else
	{
		Ability->PlayMontage(Montage, SectionName, PlayRate);
	}
}

void UGAAbilityTask_PlayMontage::BroadcastStartNotifyState(const FGameplayTag& InTag, const FName& InName)
{
	NotifyBegin.Broadcast(InTag, InName);
}
void UGAAbilityTask_PlayMontage::BroadcastEndNotifyState(const FGameplayTag& InTag, const FName& InName)
{
	NotifyTick.Broadcast(InTag, InName);
}
void UGAAbilityTask_PlayMontage::BroadcastTickNotifyState(const FGameplayTag& InTag, const FName& InName)
{
	NotifyEnd.Broadcast(InTag, InName);
}