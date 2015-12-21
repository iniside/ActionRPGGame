// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../GASAbilitiesComponent.h"
#include "../GASAbilityBase.h"
#include "GASThreeButtonCombo.h"




void UGASThreeButtonCombo::NativeInputPressed(int32 Index)
{
	//Index = 0 LMB
	//Index = 1 RMB
	//wait for confirm ?
	UE_LOG(GameAbilities, Log, TEXT("Three Button Combo Current Index = %s"), *FString::FormatAsNumber(CurrentAbilityIdx));
	if (CurrentAbilityIdx > 2)
	{
		CurrentAbilityIdx = 0;
	}
	if (Index == 0)
	{
		AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index].ActiveAbilities[CurrentAbilityIdx]->OnNativeInputPressed();
		CurrentAbilityIdx++;
	}
	else if (Index == 1)
	{
		if(CurrentAbilityIdx == 1)
			AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index].ActiveAbilities[CurrentAbilityIdx]->OnNativeInputPressed();
	}
}
void UGASThreeButtonCombo::NativeInputReleased(int32 Index)
{
	if (Index == 0)
	{
		AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index].ActiveAbilities[CurrentAbilityIdx]->OnNativeInputReleased();
		CurrentAbilityIdx++;
		if (CurrentAbilityIdx > 3)
		{
			CurrentAbilityIdx = 0;
		}
	}
	else if (Index == 1)
	{
		if(CurrentAbilityIdx == 1)
			AbilityComp->ActiveAbilityContainer.AbilitySets[SetIndex].Abilities[Index].ActiveAbilities[CurrentAbilityIdx]->OnNativeInputReleased();
	}
}