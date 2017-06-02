// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFAttributeDurationInfinite.h"




bool UAFAttributeDurationInfinite::ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer)
{
	InContainer->AddEffect(InHandle, true);
	EffectIn->Context.TargetComp->ExecuteEffect(InHandle, InProperty);
	return true;
}