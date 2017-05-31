// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "../../GAAbilitiesComponent.h"
#include "AFAttributeDurationOverride.h"




bool UAFAttributeDurationOverride::ApplyEffect(const FGAEffectHandle& InHandle, const struct FGAEffect& EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer)
{
	TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(InHandle);
	for (const FGAEffectHandle& handle : handles)
	{
		InContainer->RemoveEffect(handle);
	}
	//if (!InHandle.GetWithPeriod())
	{
		handles = InContainer->GetHandlesByAttribute(InHandle);
		for (const FGAEffectHandle& handle : handles)
		{
			InContainer->RemoveEffect(handle);
		}
	}
	InContainer->RemoveEffect(InHandle);

	FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UGAAbilitiesComponent::ExpireEffect, InHandle);
	DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
		EffectIn.GetDurationTime(), false);

	InContainer->AddEffect(InHandle);
	EffectIn.Context.TargetComp->ExecuteEffect(InHandle, InProperty);
	return true;
}