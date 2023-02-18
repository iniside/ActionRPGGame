// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "AFEffectsComponent.h"
#include "AFPeriodApplicationOverride.h"




bool UAFPeriodApplicationOverride::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{

	InContainer->RemoveEffect(Params.Property, Params.GetContext());

	FTimerManager& DurationTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, Params);
	DurationTimer.SetTimer(const_cast<FGAEffect&>(EffectIn).DurationTimerHandle, delDuration,
		Params.GetProperty().GetDuration(), false);

	FTimerManager& PeriodTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

	FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExecuteEffect, InHandle, Params, Modifier);
	PeriodTimer.SetTimer(const_cast<FGAEffect&>(EffectIn).PeriodTimerHandle, PeriodDuration,
		Params.GetProperty().GetPeriod(), true);

	return true;
}