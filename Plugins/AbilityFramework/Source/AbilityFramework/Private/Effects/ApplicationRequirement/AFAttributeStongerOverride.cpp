// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../../Attributes/GAAttributeBase.h"
#include "../../AFAbilityInterface.h"
#include "AFAttributeStongerOverride.h"




bool UAFAttributeStongerOverride::CanApply(
	const FGAEffect& EffectIn
	, const FAFEffectParams& Params
	, const FGAEffectHandle& InHandle
	, struct FGAEffectContainer* InContainer)
{
	bool bCanApply = true;
	FGAAttribute Attribute = Params.GetProperty().GetSpecData()->AtributeModifier.Attribute;
	FAFAttributeBase* AttributePtr = Params.GetContext().TargetInterface->GetAttribute(Attribute);
	FGAEffectProperty& InProperty = Params.GetProperty();

	if (AttributePtr)
	{
		FGAEffectMod mod = FAFStatics::GetAttributeModifier(InProperty.GetAttributeModifier()
			, InProperty.GetSpecData()
			, Params.GetContext()
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