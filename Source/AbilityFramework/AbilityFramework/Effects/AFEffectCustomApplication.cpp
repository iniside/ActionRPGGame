// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectCustomApplication.h"




bool UAFEffectCustomApplication::ApplyEffect(const FGAEffectHandle& InHandle, FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, struct FGAEffectContainer* InContainer)
{
	InHandle.GetContext().TargetComp->ExecuteEffect(InHandle, InProperty);
	return true;
}