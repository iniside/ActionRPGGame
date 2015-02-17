// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
#include "Effects/GAEffectSpecification.h"
#include "GAAttributes.h"


void FGAAttributeBase::AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle)
{
	TArray<FGAModifier>& modsTemp = Modifiers.FindOrAdd(Handle);
	modsTemp.Add(ModifiersIn);
	CalculateBonus();
}
void FGAAttributeBase::RemoveBonus(const FGAEffectHandle& Handle)
{
	Modifiers.Remove(Handle);
	CalculateBonus();
}
void FGAAttributeBase::CalculateBonus()
{
	float AdditiveBonus = 0;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		for (const FGAModifier& mod : ModIt->Value)
		{
			switch (mod.AttributeMod)
			{
			case EGAAttributeMod::Add:
				AdditiveBonus += mod.Value;
				break;
			default:
				break;
			}
		}
	}
	float OldBonus = BonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	BonusValue = AdditiveBonus;
	//this is absolute maximum (not clamped right now).
	float addValue = BonusValue - OldBonus;
	//reset to max = 200
	CurrentValue = CurrentValue + addValue;
	/*
	BaseValue = 200;
	CurrentValue = 200;
	BonusValue = 50;
	CurrentValue = 200 + 50;
	CurentValue == 250;

	Damage taken.
	CurrentValue = 250 - 25;
	CurrentValue == 225;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 175; (225 - 50)
	OldBonusValue = 50;
	BonusValue = 0;
	CurrentValue == 225;
	BonusValue - OldBonusValue = -50;
	CurrentValue = CurrentValue + (-50); ??

	TwoBonuses 50 + 50;
	CurrentValue = 300 - 25;
	CurrentValue == 275;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 225; (275 - 50)
	OldBonusValue = 100;
	BonusValue = 50;
	CurrentValue == 225;
	BonusValue - OldBonusValue = -50;
	CurrentValue = CurrentValue + (-50); ??

	Inverse Bonus is going to be Increased:
	TwoBonuses 50 + 50;
	CurrentValue = 300 - 25;
	CurrentValue == 275;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 325; (275 + 50)
	OldBonusValue = 100;
	new BonusValue = 150; (new bonus gives +50)
	CurrentValue == 275;
	BonusValue - OldBonusValue = 50; (150 - 100) = 50
	CurrentValue = CurrentValue + (50); ??
	*/
}
void FGAAttributeBase::UpdateAttribute()
{

}
void FGAAttributeBase::Add(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	float Val = CurrentValue - (OldCurrentValue + ValueIn);
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, BaseValue);
}
void FGAAttributeBase::Subtract(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	float Val = CurrentValue - (OldCurrentValue - ValueIn);
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
}

void FGAAttributeBase::InitializeAttribute()
{
	CurrentValue = GetFinalValue();
	CalculateBonus();
}

float FGAModifierMagnitude::GetMagnitude(const FGAEffectContext& Context)
{
	FGAAttributeBase* attr = nullptr;
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
	float Result = CurveTable.Eval(attr->GetFinalValue());

	return Result;
}

float FGAAttributeBasedModifier::GetValue(const FGAEffectContext& Context)
{
	FGAAttributeBase* attr = nullptr;
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
	float Result = (Coefficient * (PreMultiply + attr->GetFinalValue()) + PostMultiply) * PostCoefficient;
	if (attr && !bUseSecondaryAttribute)
		return Result;

	switch (SecondarySource)
	{
		case EGAAttributeSource::Instigator:
			attr = Context.InstigatorComp->GetAttribute(SecondaryAttribute);
			break;
		case EGAAttributeSource::Target:
			attr = Context.TargetComp->GetAttribute(SecondaryAttribute);
			break;
		default:
			return Result;
	}

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

	float Result = CurveTable.Eval(attr->GetFinalValue());
	return Result;
}

FGAAttributeData FGAAttributeModifier::GetModifier(const FGAEffectContext& ContextIn)
{
	switch (CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
		return FGAAttributeData(Attribute, Mod, AttributeTags, DirectModifier.GetValue());
	case EGAMagnitudeCalculation::AttributeBased:
		return FGAAttributeData(Attribute, Mod, AttributeTags, AttributeBased.GetValue(ContextIn));
	case EGAMagnitudeCalculation::CurveBased:
		return FGAAttributeData(Attribute, Mod, AttributeTags, CurveBased.GetValue(ContextIn));;
	case EGAMagnitudeCalculation::CustomCalculation:
		break;
	default:
		break;
	}
	return FGAAttributeData(FGAAttribute(NAME_None), EGAAttributeMod::Invalid, FGameplayTagContainer(), -1);
}