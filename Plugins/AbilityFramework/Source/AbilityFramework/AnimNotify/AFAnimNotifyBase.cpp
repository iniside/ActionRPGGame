// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"
#include "AFAnimNotifyBase.h"




void UAFAnimNotifyBase::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IAFAbilityInterface* IAbilities = Cast<IAFAbilityInterface>(MeshComp->GetOwner());
	if (!IAbilities)
		return;

	UAFAbilityComponent* Comp = IAbilities->GetAbilityComp();
}