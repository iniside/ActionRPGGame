// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "GAGlobalTypes.h"
#include "Effects/GAGameEffect.h"
#include "AFEffectsComponent.h"
#include "AFPeriodApplicationExtend.h"




bool UAFPeriodApplicationExtend::ApplyEffect(
	const FGAEffectHandle& InHandle
	, const FGAEffect& EffectIn
	, struct FGAEffectContainer* InContainer
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(Params.GetProperty(), Params.GetContext());
	for (const FGAEffectHandle& handle : handles)
	{
		FGAEffect& ExtEffect = *InContainer->GetEffect(handle);
		FGAEffect& Effect = const_cast<FGAEffect&>(EffectIn);
		
		FGAEffectContext& ExtContext = const_cast<FAFEffectParams&>(Params).GetContext();

		FTimerManager& DurationTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

		float RemainingTime = DurationTimer.GetTimerRemaining(ExtEffect.DurationTimerHandle);
		float NewDuration = RemainingTime + Effect.GetDurationTime();
		DurationTimer.ClearTimer(ExtEffect.DurationTimerHandle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(ExtContext.GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, Params);
		DurationTimer.SetTimer(ExtEffect.DurationTimerHandle, delDuration,
			NewDuration, false);
	}
	if (handles.Num() <= 0)
	{
		FGAEffect& Effect = const_cast<FGAEffect&>(EffectIn);;
		FTimerManager& DurationTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, Params);
		DurationTimer.SetTimer(Effect.DurationTimerHandle, delDuration,
			Params.GetProperty().GetDuration(), false);

		FTimerManager& PeriodTimer = const_cast<FAFEffectParams&>(Params).GetTargetTimerManager();

		FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(Params.GetTargetEffectsComponent(), &UAFEffectsComponent::ExecuteEffect, InHandle, Params, Modifier);
		PeriodTimer.SetTimer(Effect.PeriodTimerHandle, PeriodDuration,
			Params.GetProperty().GetPeriod(), true);
	}
	return true;
}