// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityData.h"




TArray<FGameplayTag> UARAbilityData::GetAbilitiesTags()
{
	TArray<FGameplayTag> Out;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		Out.Add(It->Key);
	}
	return Out;
}