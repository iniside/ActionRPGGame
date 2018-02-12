// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "AFAbilityNotifyState.h"




void UAFAbilityNotifyState::NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	IAFAbilityInterface* IAbilities = Cast<IAFAbilityInterface>(MeshComp->GetOwner());
	if (IAbilities)
	{
		CachedAbilitiesComp = IAbilities->GetAbilityComp();
		CachedAbilitiesComp->OnAbilityNotifyBegin.ExecuteIfBound(Tag, Name);
	}
}
void UAFAbilityNotifyState::NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyTick.ExecuteIfBound(Tag, Name);
	}
}
void UAFAbilityNotifyState::NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyEnd.ExecuteIfBound(Tag, Name);
	}
}