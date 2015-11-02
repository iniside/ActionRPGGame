#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"

#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"

#include "GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GACustomCalculation.h"

FGAHashedGameplayTagContainer::FGAHashedGameplayTagContainer(const FGameplayTagContainer& TagsIn)
	: Tags(TagsIn)
{
	GenerateFNameKey();
}
void FGAHashedGameplayTagContainer::GenerateFNameKey()
{
	FString RetString;
	for (const FGameplayTag& tag : Tags)
	{
		RetString += TEXT(".");
		RetString += tag.ToString();
		RetString += TEXT(".");
	}
	Key = *RetString;
}
FGAEffectHandle FGAEffectHandle::GenerateHandle()
{
	static int32 GlobalHandleID = 0;
	FGAEffectHandle NewHandle(GlobalHandleID++);

	return NewHandle;
}
FGAModifier::FGAModifier(EGAAttributeMod ModIn, float ValueIn, TSharedPtr<struct FGAActiveDuration> ActiveEffect)
	: AttributeMod(ModIn),
	Value(ValueIn)
{
	Effect = TWeakPtr<FGAActiveDuration>(ActiveEffect);
}
void FGAEffectContext::Reset()
{
	Target.Reset();
	Causer.Reset();
	Instigator.Reset();
	TargetComp.Reset();
	InstigatorComp.Reset();
}
class UGAAttributesBase* FGAEffectContext::GetTargetAttributes()
{ 
	return TargetComp->DefaultAttributes; 
}
class UGAAttributesBase* FGAEffectContext::GetInstigatorAttributes()
{ 
	return InstigatorComp->DefaultAttributes; 
}
FGAEffectContext::~FGAEffectContext()
{
	Target.Reset();
	Causer.Reset();
	Instigator.Reset();
	TargetComp.Reset();
	InstigatorComp.Reset();
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
void FGACountedTagContainer::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	for (auto TagIt = TagsIn.CreateConstIterator(); TagIt; ++TagIt)
	{
		int32* count = CountedTags.Find(*TagIt);
		if (count)
		{
			*count -= 1;
		}
		if (*count <= 0)
		{
			CountedTags.Remove(*TagIt);
			AllTags.RemoveTag(*TagIt);
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