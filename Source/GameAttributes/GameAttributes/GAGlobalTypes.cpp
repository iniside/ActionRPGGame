#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
#include "Effects/GAEffectSpecification.h"
#include "GAGlobalTypes.h"

FGAEffectHandle FGAEffectHandle::GenerateHandle()
{
	static int32 GlobalHandleID = 0;
	FGAEffectHandle NewHandle(GlobalHandleID++);

	return NewHandle;
}


void FGAAttributeBase::AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle)
{
	Modifiers.Add(Handle, ModifiersIn);
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
		switch (ModIt->Value.AttributeMod)
		{
		case EGAAttributeMod::Add:
			AdditiveBonus += ModIt->Value.Value;
				break;
			default:
				break;
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

FGAEvalData FGAAttributeModifier::GetModifier(const FGAEffectContext& ContextIn)
{
	switch (CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
		return FGAEvalData(Attribute, Mod, AttributeTag, DirectModifier.GetValue());
	case EGAMagnitudeCalculation::AttributeBased:
		return FGAEvalData(Attribute, Mod, AttributeTag, AttributeBased.GetValue(ContextIn));
	case EGAMagnitudeCalculation::CurveBased:
		return FGAEvalData(Attribute, Mod, AttributeTag, CurveBased.GetValue(ContextIn));;
	case EGAMagnitudeCalculation::CustomCalculation:
		break;
	default:
		break;
	}
	return FGAEvalData(FGAAttribute(NAME_None), EGAAttributeMod::Invalid, FGameplayTag(AttributeTag), -1);
}

FGAEffectSpec::FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn, const FGAEffectContext& ContextIn)
{

}
FGAEffectSpec::FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
	FGAEffectDuration DurationIn, const FGAEffectContext& ContextIn)
{
	EffectDuration = DurationIn;
}
FGAEffectSpec::FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
	TArray<FGAAttributeModifier> ModifiersIn, const FGAEffectContext& ContextIn)
{
	AttributeModifiers = ModifiersIn;
}
/*
Constructor which will override only specific modifiers.
*/
FGAEffectSpec::FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
	TArray<FGAModifierOverride> OverridesIn, const FGAEffectContext& ContextIn)
{
	for (const FGAModifierOverride& mo : OverridesIn)
	{
		for (FGAAttributeModifier& am : AttributeModifiers)
		{
			if (mo.Attribute == am.Attribute)
			{
				am = mo.Modifier;
			}
		}
	}
}
TArray<FGAEvalData> FGAEffectSpec::GetModifiers()
{
	for (FGAAttributeModifier& mod : AttributeModifiers)
	{
		EvalModifiers.Add(mod.GetModifier(Context));
	}
	return EvalModifiers;
}

void FGACountedTagContainer::AddTag(const FGameplayTag& TagIn)
{
	int32* count = CountedTags.Find(TagIn);
	if (count)
	{
		*count += 1;
		return;
	}
	CountedTags.Add(TagIn, 1);
	AllTags.AddTag(TagIn);
}
void FGACountedTagContainer::AddTagContainer(const FGameplayTagContainer& TagsIn)
{
	for (auto TagIt = TagsIn.CreateConstIterator(); TagIt; ++TagIt)
	{
		int32* count = CountedTags.Find(*TagIt);
		if (count)
		{
			*count += 1;
		}
		else
		{
			CountedTags.Add(*TagIt, 1);
			AllTags.AddTag(*TagIt);
		}
	}
}
void FGACountedTagContainer::RemoveTag(const FGameplayTag& TagIn)
{
	int32* count = CountedTags.Find(TagIn);
	if (count)
	{
		*count -= 1;
		if (*count <= 0)
		{
			CountedTags.Remove(TagIn);
			AllTags.RemoveTag(TagIn);
		}
	}
}
bool FGACountedTagContainer::HasTag(const FGameplayTag& TagIn)
{
	return AllTags.HasTag(TagIn, EGameplayTagMatchType::IncludeParentTags, EGameplayTagMatchType::Explicit);
}
bool FGACountedTagContainer::HasAllTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch)
{
	return AllTags.MatchesAll(TagsIn, bCountEmptyAsMatch);
}
bool FGACountedTagContainer::HasAnyTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch)
{
	return AllTags.MatchesAny(TagsIn, bCountEmptyAsMatch);
}

FGAEffectInstant::FGAEffectInstant(const FGAEffectSpec& SpecIn, const FGAEffectContext ContextIn)
{
	Modifiers = SpecIn.EvalModifiers;
}

void FGAActiveEffectContainer::ApplyEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Ctx)
{
	/*
		Instant effects, are just applied and forggoten.
		Duration effects, are applied, and tracked (most complicated case).
		Infinite effects are applied, and tracked. It's slightly less complicated than Duration,
		since they leave as long as player decide to remove them explicitly, for example, by removing item.

		Before we will apply any effects, though we need to accumulate tags.
		From instigator, from target, from effect spec, from Causer, into single container.
	*/
	switch (SpecIn.Policy.Type)
	{
		case EGAEffectType::Instant:
		{
			FGAEffectInstant instntEffect(SpecIn, Ctx);
			HandleInstantEffect(instntEffect, Ctx);
			break;
		}
		case EGAEffectType::Duration:
		{
			break;
		}
		case EGAEffectType::Infinite:
		{
			break;
		}
		default:
		{
			return;
		}
	}
}
void FGAActiveEffectContainer::HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx)
{
	/*
		We need to check against existing effects.
	*/
	Ctx.InstigatorComp->ModifyAttributesOnTarget(SpecIn.Modifiers, Ctx, FGAEffectHandle());
}