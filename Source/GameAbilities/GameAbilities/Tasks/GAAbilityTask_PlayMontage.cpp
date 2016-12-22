// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "Entity/GASAbilityMod.h"
#include "GAAbilitiesComponent.h"
#include "../AbilityCues/GAAbilityCue.h"
#include "GAAbilityTask_PlayMontage.h"

UGAAbilityTask_PlayMontage* UGAAbilityTask_PlayMontage::AbilityPlayMontage(UObject* WorldContextObject, UAnimMontage* MontageIn, FName SectionNameIn, float PlayRateIn)
{
	auto MyObj = NewAbilityTask<UGAAbilityTask_PlayMontage>(WorldContextObject);
	MyObj->Montage = MontageIn;
	MyObj->SectionName = SectionNameIn;
	MyObj->PlayRate = PlayRateIn;
	return MyObj;
}

void UGAAbilityTask_PlayMontage::Activate()
{
	//AbilityComponent->OnAbilityStartNotify.Clear();
	//AbilityComponent->OnAbilityEndNotify.Clear();
	//AbilityComponent->OnAbilityNotify.Clear();
	//AbilityComponent->OnAbilityStartNotify.AddUObject(this, &UGAAbilityTask_PlayMontage::BroadcastStartNotify);
	//AbilityComponent->OnAbilityEndNotify.AddUObject(this, &UGAAbilityTask_PlayMontage::BroadcastEndNotify);
	//AbilityComponent->OnAbilityNotify.AddUObject(this, &UGAAbilityTask_PlayMontage::BroadcastGenericNotify);
	AbilityComponent->OnAbilityStartNotify.Unbind();
	AbilityComponent->OnAbilityEndNotify.Unbind();
	AbilityComponent->OnAbilityNotify.Unbind();
	AbilityComponent->OnAbilityNotifyStateStart.Unbind();
	AbilityComponent->OnAbilityNotifyStateTick.Unbind();
	AbilityComponent->OnAbilityNotifyStateEnd.Unbind();
	
	AbilityComponent->OnAbilityStartNotify.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastStartNotify);
	AbilityComponent->OnAbilityEndNotify.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastEndNotify);
	AbilityComponent->OnAbilityNotify.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastGenericNotify);
	
	AbilityComponent->OnAbilityNotifyStateStart.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastStartNotifyState);
	AbilityComponent->OnAbilityNotifyStateTick.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastTickNotifyState);
	AbilityComponent->OnAbilityNotifyStateEnd.BindUObject(this, &UGAAbilityTask_PlayMontage::BroadcastEndNotifyState);
	
	Ability->PlayMontage(Montage, SectionName, PlayRate);

	
	Played.Broadcast();
}

void UGAAbilityTask_PlayMontage::BroadcastStartNotify(const FGASAbilityNotifyData& DataIn)
{
	StartNotify.Broadcast();
	if(Ability->Cue)
		Ability->Cue->OnAbilityStartNotify();

	DataStart = DataIn;
}
void UGAAbilityTask_PlayMontage::BroadcastEndNotify(const FGASAbilityNotifyData& DataIn)
{
	EndNotify.Broadcast();
	DataEnd = DataIn;
	if (Ability->Cue)
		Ability->Cue->OnAbilityEndNotify();
	EndTask();
}
void UGAAbilityTask_PlayMontage::BroadcastGenericNotify(const FGASAbilityNotifyData& DataIn)
{
	GenericNotify.Broadcast();
	if (Ability->Cue)
		Ability->Cue->OnAbilityNotify();
	DataGeneric = DataIn;
}
void UGAAbilityTask_PlayMontage::BroadcastStartNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateStart.Broadcast();
	if (Ability->Cue)
		Ability->Cue->OnAbilityNotifyStateStart();
}
void UGAAbilityTask_PlayMontage::BroadcastEndNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateTick.Broadcast();
	if (Ability->Cue)
		Ability->Cue->OnAbilityNotifyStateTick();
}
void UGAAbilityTask_PlayMontage::BroadcastTickNotifyState(const FGASAbilityNotifyData& DataIn)
{
	NotifyStateTick.Broadcast();
	if (Ability->Cue)
		Ability->Cue->OnAbilityNotifyStateEnd();
}