// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "../../GAAbilitiesComponent.h"
#include "AFPeriodApplicationExtend.h"




bool UAFPeriodApplicationExtend::ApplyEffect(const FGAEffectHandle& InHandle, const struct FGAEffect& EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer)
{
	TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(InHandle);
	for (const FGAEffectHandle& handle : handles)
	{
		FGAEffect& ExtEffect = handle.GetEffectRef();
		FGAEffect& Effect = InHandle.GetEffectRef();
		FTimerManager& DurationTimer = handle.GetContext().TargetComp->GetWorld()->GetTimerManager();
		float RemainingTime = DurationTimer.GetTimerRemaining(handle.GetEffectPtr()->DurationTimerHandle);
		float NewDuration = RemainingTime + Effect.GetDurationTime();
		DurationTimer.ClearTimer(handle.GetEffectPtr()->DurationTimerHandle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(handle.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, handle);
		DurationTimer.SetTimer(handle.GetEffectPtr()->DurationTimerHandle, delDuration,
			NewDuration, false);
	}
	if (handles.Num() <= 0)
	{
		FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, InHandle);
		DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
			EffectIn.GetDurationTime(), false);

		FTimerManager& PeriodTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

		FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExecuteEffect, InHandle, InProperty);
		PeriodTimer.SetTimer(InHandle.GetEffectPtr()->PeriodTimerHandle, PeriodDuration,
			InProperty.Period, true);

		InContainer->AddEffect(InHandle);
	}
	return true;
}