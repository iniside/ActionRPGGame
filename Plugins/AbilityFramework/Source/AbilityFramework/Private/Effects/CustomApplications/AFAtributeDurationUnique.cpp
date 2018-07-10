// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "AFEffectsComponent.h"
#include "AFAbilityInterface.h"
#include "AFAtributeDurationUnique.h"




bool UAFAtributeDurationUnique::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	if (InContainer->IsEffectActive(Params.Property.SpecClass.SpecClass.Get()))
	{
		return false;
	}
	FTimerManager& DurationTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, Params);
	DurationTimer.SetTimer(const_cast<FGAEffect&>(EffectIn).DurationTimerHandle, delDuration,
		Params.GetProperty().GetDuration(), false);

	return true;
}

bool UAFAtributeDurationUnique::CanApply(class IAFAbilityInterface* Target, TSubclassOf<UGAGameEffectSpec> EffectClass)
{
	if (!Target)
		return false;

	return !Target->GetEffectsComponent()->IsEffectActive(EffectClass);
}