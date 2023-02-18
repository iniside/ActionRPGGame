// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "AFEffectsComponent.h"
#include "AFAtributeDurationAdd.h"




bool UAFAtributeDurationAdd::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	FTimerManager& DurationTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, Params);
	DurationTimer.SetTimer(const_cast<FGAEffect&>(EffectIn).DurationTimerHandle, delDuration,
		Params.GetProperty().GetDuration(), false);

	return true;
}