#pragma once
#include "GameAttributes.h"
#include "GAGlobalTypes.h"
#include "GAAttributeComponent.h"
#include "GAAttributeBase.h"
#include "GAEffectExecution.h"
#include "GACustomCalculation.h"

FGAAttributeBase* FGAExecutionContext::GetTargetAttribute(const FGAAttribute& AttributeIn)
{
	return TargetAttributes->GetAttribute(AttributeIn);
}
FGAAttributeBase* FGAExecutionContext::GetInstigatorAttribute(const FGAAttribute& AttributeIn)
{
	return InstigatorAttributes->GetAttribute(AttributeIn);
}

float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context)
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
		attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	case EGAAttributeSource::Target:
		attr = Context.TargetComp->GetAttribute(Attribute);
		break;
	default:
		return 0;
	}
	Result = (Coefficient * (PreMultiply + attr->GetFinalValue()) + PostMultiply) * PostCoefficient;
	if (!bUseSecondaryAttribute)
		return Result;

	//switch (SecondarySource)
	//{
	//	case EGAAttributeSource::Instigator:
	//		attr = Context.InstigatorComp->GetAttribute(SecondaryAttribute);
	//		break;
	//	case EGAAttributeSource::Target:
	//		attr = Context.TargetComp->GetAttribute(SecondaryAttribute);
	//		break;
	//	default:
	//		return Result;
	//}

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
	Result = CurveTable.Eval(attr->GetFinalValue());
	return Result;
}

float FGACustomCalculationModifier::GetValue(const FGAEffectContext& ContextIn)
{
	if (CustomCalculation)
	{
		return CustomCalculation->NativeCalculateMagnitude(ContextIn);
	}
	return 0;
}