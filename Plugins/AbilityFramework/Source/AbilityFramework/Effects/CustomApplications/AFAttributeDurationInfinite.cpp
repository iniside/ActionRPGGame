// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAttributeDurationInfinite.h"




bool UAFAttributeDurationInfinite::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	return true;
}