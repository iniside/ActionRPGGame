// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilitiesComponent.h"
#include "GASAbilityBase.h"
#include "GASInputDefault.h"




void UGASInputDefault::NativeInputPressed(int32 Index)
{
	OnInputPressed(Index);
	UE_LOG(GameAbilities, Log, TEXT("Using custom input pressed for set: %d"), SetIndex);
	FGASActiveAbility& Ability = AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index];
	FGASActiveAbilityContainer& ActiveAbilityContainer = AbilityComp->ActiveAbilityContainer;
	if (ActiveAbilityContainer.CanUseAbility(SetIndex, 0))
	{
		//waiting for confirmation is exclusive at least now, ability either wait or not.
		if (ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, 0))
		{
			UE_LOG(GameAbilities, Log, TEXT("CustomInput: Confirm Ability : %d"), SetIndex);
			ActiveAbilityContainer.ConfirmAbility(SetIndex, 0);
		}
		else
		{
			UE_LOG(GameAbilities, Log, TEXT("CustomInput: InputPressed : %d"), SetIndex);
			Ability.ActiveAbilities[0]->OnNativeInputPressed();
			//ActiveAbilityContainer.OnNativeInputPressed(SetIndex, SlotIndex);
		}
	}
	//Ability.ActiveAbilities[0]->OnNativeInputPressed();
}
void UGASInputDefault::NativeInputReleased(int32 Index)
{
	OnInputReleased(Index);
	UE_LOG(GameAbilities, Log, TEXT("Using custom input released for set: %d"), SetIndex);
	FGASActiveAbility& Ability = AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index];
	FGASActiveAbilityContainer& ActiveAbilityContainer = AbilityComp->ActiveAbilityContainer;
	//Ability.ActiveAbilities[0]->OnNativeInputReleased();
	if (!ActiveAbilityContainer.IsValid(SetIndex, 0))
		return;

	if (ActiveAbilityContainer.CanUseAbility(SetIndex, 0))
	{
		if (!ActiveAbilityContainer.IsWaitingForConfirm(SetIndex, 0))
		{
			UE_LOG(GameAbilities, Log, TEXT("CustomInput: Input Released : %d"), SetIndex);
			Ability.ActiveAbilities[0]->OnNativeInputReleased();
		}
	}
}