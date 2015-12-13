// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilitiesComponent.h"
#include "GASInputOverride.h"




void UGASInputOverride::NativeInputPressed(int32 Index)
{
	UE_LOG(GameAbilities, Log, TEXT("Using custom input pressed for set: %f"), SetIndex);
	FGASActiveAbility& Ability = AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index];
	Ability.ActiveAbilities[0]->OnNativeInputPressed();
}
void UGASInputOverride::NativeInputReleased(int32 Index)
{
	UE_LOG(GameAbilities, Log, TEXT("Using custom input released for set: %f"), SetIndex);
	FGASActiveAbility& Ability = AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index];
	Ability.ActiveAbilities[0]->OnNativeInputReleased();
}