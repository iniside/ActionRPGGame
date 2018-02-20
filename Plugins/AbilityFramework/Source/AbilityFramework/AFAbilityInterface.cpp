// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "AFEffectsComponent.h"
#include "AFAttributeComponent.h"
UAFAbilityInterface::UAFAbilityInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//class UGAAttributesBase* IAFAbilityInterface::GetAttributes()
//{
//	return nullptr;// GetAbilityComp()->DefaultAttributes;
//}

FGAEffectHandle IAFAbilityInterface::ApplyEffectToTarget(
	const FGAEffect& EffectIn
	, const FGAEffectHandle& InHandle
	, const FAFEffectParams& Params
	, const FAFFunctionModifier& Modifier)
{
	return GetEffectsComponent()->ApplyEffectToTarget(EffectIn, InHandle, Params, Modifier);
}