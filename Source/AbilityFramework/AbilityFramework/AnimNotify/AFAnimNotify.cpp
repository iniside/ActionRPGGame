// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "GAAbilitiesComponent.h"
#include "IGAAbilities.h"
#include "AFAnimNotify.h"




void UAFAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IIGAAbilities* IAbilities = Cast<IIGAAbilities>(MeshComp->GetOwner());
	if (!IAbilities)
		return;

	UGAAbilitiesComponent* Comp = IAbilities->GetAbilityComp();
	Comp->OnAbilityNotifyBegin.ExecuteIfBound(Data, Tag, Name);
}