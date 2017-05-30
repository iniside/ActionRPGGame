// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "AFEffectCustomStackingRule.h"




bool UAFEffectCustomStackingRule::CanStack(class UGAAbilitiesComponent* InComp, struct FGAEffectContainer* InContainer,
	const FGAEffectHandle& InHandle)
{
	//InHandle.GetContext().TargetComp->ExecuteEffect(InHandle);
	return true;
}