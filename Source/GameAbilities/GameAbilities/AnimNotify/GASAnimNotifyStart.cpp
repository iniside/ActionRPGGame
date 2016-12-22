// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GAAbilitiesComponent.h"
#include "IGAAbilities.h"
#include "GASAnimNotifyStart.h"




void UGASAnimNotifyStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IIGAAbilities* IAbilities = Cast<IIGAAbilities>(MeshComp->GetOwner());
	if (!IAbilities)
		return;

	UGAAbilitiesComponent* Comp = IAbilities->GetAbilityComp();
	Comp->OnAbilityStartNotify.ExecuteIfBound(Data);
}