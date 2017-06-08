// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectAlreadyApplied.h"




bool UAFEffectAlreadyApplied::CanApply(FGAEffect* EffectIn, FGAEffectProperty& InProperty,
	struct FGAEffectContainer* InContainer)
{
	if (InContainer->ContainsEffectOfClass(InProperty))
	{
		return false;
	}
	return true;
}