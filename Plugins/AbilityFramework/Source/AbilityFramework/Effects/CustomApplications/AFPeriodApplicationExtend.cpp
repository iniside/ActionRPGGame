// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "../../AFAbilityComponent.h"
#include "AFPeriodApplicationExtend.h"




bool UAFPeriodApplicationExtend::ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext, const FAFFunctionModifier& Modifier)
{
	TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(InProperty, EffectIn->Context);
	for (const FGAEffectHandle& handle : handles)
	{
		FGAEffect& ExtEffect = handle.GetEffectRef();
		FGAEffect& Effect = InHandle.GetEffectRef();
		FTimerManager& DurationTimer = handle.GetContext().TargetComp->GetWorld()->GetTimerManager();
		float RemainingTime = DurationTimer.GetTimerRemaining(handle.GetEffectPtr()->DurationTimerHandle);
		float NewDuration = RemainingTime + Effect.GetDurationTime();
		DurationTimer.ClearTimer(handle.GetEffectPtr()->DurationTimerHandle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(handle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExpireEffect, InHandle, InProperty, InContext);
		DurationTimer.SetTimer(handle.GetEffectPtr()->DurationTimerHandle, delDuration,
			NewDuration, false);
	}
	if (handles.Num() <= 0)
	{
		FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExpireEffect, InHandle, InProperty, InContext);
		DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
			InProperty.GetDuration(), false);

		FTimerManager& PeriodTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExecuteEffect, InHandle, InProperty, Modifier, InContext);
		PeriodTimer.SetTimer(InHandle.GetEffectPtr()->PeriodTimerHandle, PeriodDuration,
			InProperty.GetPeriod(), true);

		InContainer->AddEffect(InProperty, InHandle);
	}
	return true;
}