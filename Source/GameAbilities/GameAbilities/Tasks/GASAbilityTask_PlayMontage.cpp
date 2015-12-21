// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "Entity/GASAbilityMod.h"
#include "GASAbilitiesComponent.h"
#include "GASAbilityTask_PlayMontage.h"

UGASAbilityTask_PlayMontage* UGASAbilityTask_PlayMontage::AbilityPlayMontage(UObject* WorldContextObject, UAnimMontage* MontageIn, FName SectionNameIn, float PlayRateIn)
{
	auto MyObj = NewAbilityTask<UGASAbilityTask_PlayMontage>(WorldContextObject);
	MyObj->Montage = MontageIn;
	MyObj->SectionName = SectionNameIn;
	MyObj->PlayRate = PlayRateIn;
	return MyObj;
}

void UGASAbilityTask_PlayMontage::Activate()
{
	//AbilityComponent->OnAbilityStartNotify.Clear();
	//AbilityComponent->OnAbilityEndNotify.Clear();
	//AbilityComponent->OnAbilityNotify.Clear();
	//AbilityComponent->OnAbilityStartNotify.AddUObject(this, &UGASAbilityTask_PlayMontage::BroadcastStartNotify);
	//AbilityComponent->OnAbilityEndNotify.AddUObject(this, &UGASAbilityTask_PlayMontage::BroadcastEndNotify);
	//AbilityComponent->OnAbilityNotify.AddUObject(this, &UGASAbilityTask_PlayMontage::BroadcastGenericNotify);
	AbilityComponent->OnAbilityStartNotify.Unbind();
	AbilityComponent->OnAbilityEndNotify.Unbind();
	AbilityComponent->OnAbilityNotify.Unbind();
	AbilityComponent->OnAbilityNotifyStateStart.Unbind();
	AbilityComponent->OnAbilityNotifyStateTick.Unbind();
	AbilityComponent->OnAbilityNotifyStateEnd.Unbind();
	
	AbilityComponent->OnAbilityStartNotify.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastStartNotify);
	AbilityComponent->OnAbilityEndNotify.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastEndNotify);
	AbilityComponent->OnAbilityNotify.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastGenericNotify);
	
	AbilityComponent->OnAbilityNotifyStateStart.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastStartNotifyState);
	AbilityComponent->OnAbilityNotifyStateTick.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastTickNotifyState);
	AbilityComponent->OnAbilityNotifyStateEnd.BindUObject(this, &UGASAbilityTask_PlayMontage::BroadcastEndNotifyState);
	
	Ability->PlayMontage(Montage, SectionName);

	
	Played.Broadcast();
}

void UGASAbilityTask_PlayMontage::BroadcastStartNotify(const FGASAbilityNotifyData& DataIn)
{
	StartNotify.Broadcast();
	DataStart = DataIn;
}
void UGASAbilityTask_PlayMontage::BroadcastEndNotify(const FGASAbilityNotifyData& DataIn)
{
	EndNotify.Broadcast();
	DataEnd = DataIn;
	EndTask();
}
void UGASAbilityTask_PlayMontage::BroadcastGenericNotify(const FGASAbilityNotifyData& DataIn)
{
	GenericNotify.Broadcast();
	DataGeneric = DataIn;
}
void UGASAbilityTask_PlayMontage::BroadcastStartNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateStart.Broadcast();
}
void UGASAbilityTask_PlayMontage::BroadcastEndNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateTick.Broadcast();
}
void UGASAbilityTask_PlayMontage::BroadcastTickNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateTick.Broadcast();
}