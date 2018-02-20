// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectAlreadyApplied.h"




bool UAFEffectAlreadyApplied::CanApply(
	const FGAEffect& EffectIn
	, const FAFEffectParams& Params
	, const FGAEffectHandle& InHandle
	, struct FGAEffectContainer* InContainer)
{
	if (InContainer->ContainsEffectOfClass(Params.Property))
	{
		return false;
	}
	return true;
}