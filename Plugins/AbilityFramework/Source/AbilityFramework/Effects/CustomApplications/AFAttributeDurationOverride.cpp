// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "AFEffectsComponent.h"
#include "AFAttributeDurationOverride.h"




bool UAFAttributeDurationOverride::ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext,
	const FAFFunctionModifier& Modifier)
{
	InContainer->RemoveEffect(InProperty);

	FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().GetTargetEffectsComponent(), &UAFEffectsComponent::ExpireEffect, InHandle, InProperty, InContext);
	DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
		InProperty.GetDuration(), false);

	InContainer->AddEffect(InProperty, InHandle);
	return true;
}