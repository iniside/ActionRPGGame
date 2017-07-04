// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "../../AFAbilityComponent.h"
#include "AFPeriodApplicationOverride.h"




bool UAFPeriodApplicationOverride::ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext,
	const FAFFunctionModifier& Modifier)
{
	//TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(InProperty, EffectIn->Context);
	//for (const FGAEffectHandle& handle : handles)
	//{
	//	InContainer->RemoveEffect(InProperty);
	//}
	InContainer->RemoveEffect(InProperty);

	FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExpireEffect, InHandle, InProperty, InContext);
	DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
		InProperty.Duration, false);

	FTimerManager& PeriodTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

	FTimerDelegate PeriodDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExecuteEffect, InHandle, InProperty, Modifier, InContext);
	PeriodTimer.SetTimer(InHandle.GetEffectPtr()->PeriodTimerHandle, PeriodDuration,
		InProperty.Period, true);

	InContainer->AddEffect(InHandle);
	//EffectIn.Context.TargetComp->ExecuteEffect(InHandle, InProperty);
	return true;
}