// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "AFEffectCustomApplication.h"




bool UAFEffectCustomApplication::ApplyEffect(
	  const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	return true;
}

void UAFEffectCustomApplication::ApplyExecute(
	  const FGAEffectHandle& InHandle
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	Params.GetContext().GetTargetEffectsComponent()->ExecuteEffect(InHandle, Params, Modifier);
}