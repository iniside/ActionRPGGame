// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "IGASAbilities.h"
#include "GASAbilitiesComponent.h"
#include "GASAbilityNotifyState.h"




void UGASAbilityNotifyState::NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration)
{
	IIGASAbilities* IAbilities = Cast<IIGASAbilities>(MeshComp->GetOwner());
	if (IAbilities)
	{
		CachedAbilitiesComp = IAbilities->GetAbilityComp();
		CachedAbilitiesComp->OnAbilityNotifyStateStart.ExecuteIfBound(Data);
	}
}
void UGASAbilityNotifyState::NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyStateTick.ExecuteIfBound(Data);
	}
}
void UGASAbilityNotifyState::NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation)
{
	if (CachedAbilitiesComp)
	{
		CachedAbilitiesComp->OnAbilityNotifyStateEnd.ExecuteIfBound(Data);
	}
}