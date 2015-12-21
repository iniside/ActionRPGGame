// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilitiesComponent.h"
#include "IGASAbilities.h"
#include "GASAnimNotify.h"




void UGASAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IIGASAbilities* IAbilities = Cast<IIGASAbilities>(MeshComp->GetOwner());
	if (!IAbilities)
		return;

	UGASAbilitiesComponent* Comp = IAbilities->GetAbilityComp();
	Comp->OnAbilityNotify.ExecuteIfBound(Data);
}