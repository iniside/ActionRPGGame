#pragma once
#include "GameAbilities.h"
#include "GAAbilitiesComponent.h"
#include "GAAttributeBase.h"
#include "GAEffectExecution.h"
#include "IGAAbilities.h"
#include "GAGameEffect.h"
#include "GACustomCalculation.h"



float FGADirectModifier::GetValue() { return Value; }
float FGADirectModifier::GetValue() const { return Value; }
float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context)
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
	{
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Instigator.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		//attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Target:
	{
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Target.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();// nullptr;
		attr = Attributes->GetAttribute(Attribute);//Context.TargetComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Causer:
	{
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Causer.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();// nullptr;
		attr = Attributes->GetAttribute(Attribute);//Context.TargetComp->GetAttribute(Attribute);
		break;
	}
	default:
		return 0;
	}
	Result = (Coefficient * (PreMultiply + attr->GetFinalValue()) + PostMultiply) * PostCoefficient;
	if (!bUseSecondaryAttribute)
		return Result;

	float attrValue = attr->GetFinalValue();
	switch (SecondaryMod)
	{
	case EGAAttributeMagCalc::Add:
		return Result + attrValue;
	case EGAAttributeMagCalc::Subtract:
		return Result - attrValue;
	case EGAAttributeMagCalc::Multiply:
		return Result * attrValue;
	case EGAAttributeMagCalc::Divide:
		return Result / attrValue;
	case EGAAttributeMagCalc::PrecentageIncrease:
		return Result + (Result * attrValue);
	case EGAAttributeMagCalc::PrecentageDecrease:
		return Result - (Result * attrValue);
	default:
		return Result;
	}

	return 0;
}
float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context) const
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
	{	
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Instigator.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		//attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Target:
	{
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Target.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Causer:
	{
		IIGAAbilities* AttrInt = Cast<IIGAAbilities>(Context.Causer.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();// nullptr;
		attr = Attributes->GetAttribute(Attribute);
		break;
	}
	default:
		return 0;
	}
	Result = (Coefficient * (PreMultiply + attr->GetFinalValue()) + PostMultiply) * PostCoefficient;
	if (!bUseSecondaryAttribute)
		return Result;
	float attrValue = attr->GetFinalValue();
	switch (SecondaryMod)
	{
	case EGAAttributeMagCalc::Add:
		return Result + attrValue;
	case EGAAttributeMagCalc::Subtract:
		return Result - attrValue;
	case EGAAttributeMagCalc::Multiply:
		return Result * attrValue;
	case EGAAttributeMagCalc::Divide:
		return Result / attrValue;
	case EGAAttributeMagCalc::PrecentageIncrease:
		return Result + (Result * attrValue);
	case EGAAttributeMagCalc::PrecentageDecrease:
		return Result - (Result * attrValue);
	default:
		return Result;
	}

	return 0;
}
float FGASummedAttributeBasedModifier::GetValue(const FGAEffectContext& Context)
{
	float FinalValue = 0;
	for (const FGAAttributeBasedModifier& Attribute : AttributeBased)
	{
		FinalValue = FinalValue + Attribute.GetValue(Context);
	}
	return FinalValue;
}

float FGASummedAttributeBasedModifier::GetValue(const FGAEffectContext& Context) const
{
	float FinalValue = 0;
	for (const FGAAttributeBasedModifier& Attribute : AttributeBased)
	{
		FinalValue = FinalValue + Attribute.GetValue(Context);
	}
	return FinalValue;
}

float FGACurveBasedModifier::GetValue(const FGAEffectContext& ContextIn)
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
		attr = ContextIn.InstigatorComp->GetAttribute(Attribute);
		break;
	case EGAAttributeSource::Target:
		attr = ContextIn.TargetComp->GetAttribute(Attribute);
		break;
	default:
		return 0;
	}
	FString ContextString(TEXT("Evaluating modifier value."));
	Result = CurveTable.Eval(attr->GetFinalValue(), ContextString);
	return Result;
}
float FGACurveBasedModifier::GetValue(const FGAEffectContext& ContextIn) const
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
		attr = ContextIn.InstigatorComp->GetAttribute(Attribute);
		break;
	case EGAAttributeSource::Target:
		attr = ContextIn.TargetComp->GetAttribute(Attribute);
		break;
	default:
		return 0;
	}
	FString ContextString(TEXT("Evaluating modifier value."));
	Result = CurveTable.Eval(attr->GetFinalValue(), ContextString);
	return Result;
}
float FGACustomCalculationModifier::GetValue(const struct FGAEffectHandle& HandleIn)
{
	if (CustomCalculation)
	{
		return CustomCalculation->NativeCalculateMagnitude(HandleIn);
	}
	return 0;
}
float FGACustomCalculationModifier::GetValue(const struct FGAEffectHandle& HandleIn) const
{
	if (CustomCalculation)
	{
		return CustomCalculation->NativeCalculateMagnitude(HandleIn);
	}
	return 0;
}