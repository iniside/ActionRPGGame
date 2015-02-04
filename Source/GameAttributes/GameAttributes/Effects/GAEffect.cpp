// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffectComponent.h"
#include "GAEffect.h"

UGAEffect::UGAEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CurrentPeriodCount = 0;
}

void UGAEffect::Initialize()
{
	OnEffectInitialized();
}
void UGAEffect::DestroyEffect()
{
	Instigator->GetWorldTimerManager().ClearTimer(EffectPeriodTimerHandle);
	//Target = nullptr;
	Instigator = nullptr;
	Causer = nullptr;
	InstigatorEffectComponent = nullptr;
	TargetEffectComponent = nullptr;

	MarkPendingKill();
}
void UGAEffect::CallEffectPeriod()
{
	OnEffectPeriod();
	CurrentPeriodCount++;
	if (CurrentPeriodCount >= PeriodCount)
	{
		DestroyEffect();
		CurrentPeriodCount = 0;
	}
}
