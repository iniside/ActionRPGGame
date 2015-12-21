// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilitiesComponent.h"
#include "IGASAbilities.h"
#include "GASAnimNotifyStart.h"




void UGASAnimNotifyStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IIGASAbilities* IAbilities = Cast<IIGASAbilities>(MeshComp->GetOwner());
	if (!IAbilities)
		return;

	UGASAbilitiesComponent* Comp = IAbilities->GetAbilityComp();
	Comp->OnAbilityStartNotify.ExecuteIfBound(Data);
}