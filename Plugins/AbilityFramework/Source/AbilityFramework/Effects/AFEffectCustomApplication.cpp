// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "AFEffectCustomApplication.h"




bool UAFEffectCustomApplication::ApplyEffect(const FGAEffectHandle& InHandle, FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext,
	const FAFFunctionModifier& Modifier)
{
	return true;
}

void UAFEffectCustomApplication::ExecuteEffect(const FGAEffectHandle& InHandle,
	FGAEffectProperty& InProperty,
	const FGAEffectContext& InContext,
	const FAFFunctionModifier& Modifier)
{
	InHandle.GetContext().GetTargetEffectsComponent()->ExecuteEffect(InHandle, InProperty, Modifier, InContext);
}