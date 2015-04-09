// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "../GAAttributeComponent.h"
#include "GAEffectSpecification.h"

UGAEffectSpecification::UGAEffectSpecification(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

TArray<FGAAttributeData> UGAEffectSpecification::GetInitialAttribute(const FGAEffectContext& ContextIn)
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.InitialAttributes)
	{
		modsOut.Add(mod.GetModifier(ContextIn));
	}
	return modsOut;
}
TArray<FGAAttributeData> UGAEffectSpecification::GetDurationAttribute(const FGAEffectContext& ContextIn)
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.DurationAttributes)
	{
		modsOut.Add(mod.GetModifier(ContextIn));
	}
	return modsOut;
}
TArray<FGAAttributeData> UGAEffectSpecification::GetPeriodAttribute(const FGAEffectContext& ContextIn)
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.PeriodAttributes)
	{
		FGAAttributeData data = mod.GetModifier(ContextIn);
		//gather tags from effect and target.
		data.AgreggatedTags.AppendTags(MyTags);
		data.AgreggatedTags.AppendTags(AffectTags);
		data.AgreggatedTags.AppendTags(ContextIn.TargetComp->AppliedTags.GetTags());
		modsOut.Add(data);
	}
	return modsOut;
}
TArray<FGAAttributeData> UGAEffectSpecification::GetRemovedAttribute(const FGAEffectContext& ContextIn)
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.RemovedAttributes)
	{
		modsOut.Add(mod.GetModifier(ContextIn));
	}
	return modsOut;
}
TArray<FGAAttributeData> UGAEffectSpecification::GetExpiredAttribute(const FGAEffectContext& ContextIn)
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.ExpiredAttributes)
	{
		modsOut.Add(mod.GetModifier(ContextIn));
	}
	return modsOut;
}
TArray<FGAEffectModifierSpec> UGAEffectSpecification::GetEffectModifiers(const FGAEffectContext& ContextIn)
{
	TArray<FGAEffectModifierSpec> returnArray;
	for (FGAEffectModifierSpec& spec : EffectModifiers)
	{
		returnArray.Add(spec);
	}
	return returnArray;
}
