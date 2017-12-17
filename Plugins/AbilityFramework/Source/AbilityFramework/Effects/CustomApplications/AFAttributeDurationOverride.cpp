// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../GAGameEffect.h"
#include "../../AFAbilityComponent.h"
#include "AFAttributeDurationOverride.h"




bool UAFAttributeDurationOverride::ApplyEffect(const FGAEffectHandle& InHandle, struct FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext,
	const FAFFunctionModifier& Modifier)
{
	//TSet<FGAEffectHandle> handles = InContainer->GetHandlesByClass(InProperty, EffectIn->Context);
	//for (const FGAEffectHandle& handle : handles)
	//{
	//	InContainer->RemoveEffect(InProperty);
	//}
	////if (!InHandle.GetWithPeriod())
	//{
	//	handles = InContainer->GetHandlesByAttribute(InHandle);
	//	for (const FGAEffectHandle& handle : handles)
	//	{
	//		InContainer->RemoveEffect(InProperty);
	//	}
	//}
	InContainer->RemoveEffect(InProperty);

	FTimerManager& DurationTimer = InHandle.GetContext().TargetComp->GetWorld()->GetTimerManager();

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(InHandle.GetContext().TargetComp.Get(), &UAFAbilityComponent::ExpireEffect, InHandle, InProperty, InContext);
	DurationTimer.SetTimer(InHandle.GetEffectPtr()->DurationTimerHandle, delDuration,
		InProperty.Duration, false);

	InContainer->AddEffect(InProperty, InHandle);
	//EffectIn->Context.TargetComp->ExecuteEffect(InHandle, InProperty);
	return true;
}