#pragma once
#include "../AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "../Attributes/GAAttributeBase.h"
#include "GAEffectExecution.h"
#include "AFAbilityInterface.h"
#include "GAGameEffect.h"
#include "GACustomCalculation.h"



float FGADirectModifier::GetValue() { return Value; }
float FGADirectModifier::GetValue() const { return Value; }
float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context)
{
	FAFAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
	{
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Instigator.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		//attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Target:
	{
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Target.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();// nullptr;
		attr = Attributes->GetAttribute(Attribute);//Context.TargetComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Causer:
	{
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Causer.Get());
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

	return 0;
}
float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context) const
{
	FAFAttributeBase* attr = nullptr;
	float Result = 0;
	
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
	{	
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Instigator.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		//attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Target:
	{
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Target.Get());
		UGAAttributesBase* Attributes = AttrInt->GetAttributes();
		attr = Attributes->GetAttribute(Attribute);
		break;
	}
	case EGAAttributeSource::Causer:
	{
		IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Context.Causer.Get());
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

	return 0;
}

float FGACurveBasedModifier::GetValue(const FGAEffectContext& ContextIn)
{
	FAFAttributeBase* attr = nullptr;
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
	FAFAttributeBase* attr = nullptr;
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
float FGACustomCalculationModifier::GetValue(const FGAEffectContext& ContextIn)
{
	if (CustomCalculation)
	{
		return CustomCalculation->NativeCalculateMagnitude(ContextIn);
	}
	return 0;
}
float FGACustomCalculationModifier::GetValue(const FGAEffectContext& ContextIn) const
{
	if (CustomCalculation)
	{
		return CustomCalculation->NativeCalculateMagnitude(ContextIn);
	}
	return 0;
}