// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../../Attributes/GAAttributeBase.h"
#include "../../IGAAbilities.h"
#include "AFAttributeStongerOverride.h"




bool UAFAttributeStongerOverride::CanApply(FGAEffect* EffectIn, FGAEffectProperty& InProperty,
	struct FGAEffectContainer* InContainer,
	const FGAEffectContext& InContext,
	const FGAEffectHandle& InHandle)
{
	bool bCanApply = true;
	FGAAttribute Attribute = InProperty.Spec->AtributeModifier.Attribute;
	FAFAttributeBase* AttributePtr = EffectIn->Context.TargetInterface->GetAttribute(Attribute);
	if (AttributePtr)
	{
		FGAEffectMod mod = FAFStatics::GetAttributeModifier(InProperty.GetAttributeModifier()
			, InProperty.GetSpec()
			, InContext
			, InHandle);

		if (AttributePtr->CheckIfStronger(mod))
		{
			bCanApply = true;
		}
		else
		{
			bCanApply = false;
		}
	}
	return bCanApply;
}