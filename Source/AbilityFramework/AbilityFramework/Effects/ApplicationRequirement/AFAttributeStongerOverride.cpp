// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../../Attributes/GAAttributeBase.h"
#include "../../IGAAbilities.h"
#include "AFAttributeStongerOverride.h"




bool UAFAttributeStongerOverride::CanApply(const FGAEffectHandle& InHandle)
{
	bool bCanApply = true;
	FGAAttribute Attribute = InHandle.GetAttribute();
	FAFAttributeBase* AttributePtr = InHandle.GetContext().TargetInterface->GetAttribute(Attribute);
	if (AttributePtr)
	{
		if (AttributePtr->CheckIfStronger(InHandle, InHandle.GetAttributeModifier()))
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