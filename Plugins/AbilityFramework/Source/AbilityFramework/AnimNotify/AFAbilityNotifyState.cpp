// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "IGAAbilities.h"
#include "GAAbilitiesComponent.h"
#include "AFAbilityNotifyState.h"




void UAFAbilityNotifyState::NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	IIGAAbilities* IAbilities = Cast<IIGAAbilities>(MeshComp->GetOwner());
	if (IAbilities)
	{
		CachedAbilitiesComp = IAbilities->GetAbilityComp();
		CachedAbilitiesComp->OnAbilityNotifyBegin.ExecuteIfBound(Data, Tag, Name);
	}
}
void UAFAbilityNotifyState::NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyTick.ExecuteIfBound(Data, Tag, Name);
	}
}
void UAFAbilityNotifyState::NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyEnd.ExecuteIfBound(Data, Tag, Name);
	}
}