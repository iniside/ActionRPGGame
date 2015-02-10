#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
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
		case EGAAttributeOp::Add:
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
	//float Val = CurrentValue - (OldCurrentValue + ChangedValue); //either add or subtract
	//CurrentValue -= Val;
	//CurrentValue = FMath::Clamp<float>(CurrentValue, 0, BaseValue);
	//ChangedValue = 0;
}
void FGAAttributeBase::Add(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	ChangedValue += ValueIn;
	float Val = CurrentValue - (OldCurrentValue + ChangedValue); //either add or subtract
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, BaseValue);
	ChangedValue = 0;
	//UpdateAttribute(); //probabaly need separate function.
}
void FGAAttributeBase::Subtract(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	ChangedValue -= ValueIn;
	float Val = CurrentValue - (OldCurrentValue + ChangedValue); //either add or subtract
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, BaseValue);
	ChangedValue = 0;
	//UpdateAttribute();
}

void FGAAttributeBase::InitializeAttribute()
{
	CalculateBonus();
}
float FGAAttributeSpec::CalcuclateMagnitude(const FGAEffectContext& Context)
{
	float Result = 0;

	float BaseValue = -1;
	switch (AttributeMagnitude.AttributeSource)
	{
	case EGAAttributeSource::Instigator:
		BaseValue = Context.InstigatorComp->DefaultAttributes->GetFloatValue(AttributeMagnitude.AttributeMod);
		break;
	case EGAAttributeSource::Target:
		BaseValue = Context.TargetComp->DefaultAttributes->GetFloatValue(AttributeMagnitude.AttributeMod);
		break;
	default:
		break;
	}
	if (AttributeMagnitude.MagnitudeCurve.IsValid())
	{
		Result = AttributeMagnitude.MagnitudeCurve.Eval(BaseValue);
	}
	if (AttributeMagnitude.BackingAttribute.IsValid())
	{
		float AttributeValue = 0;
		switch (AttributeMagnitude.AttributeSource)
		{
		case EGAAttributeSource::Instigator:
			AttributeValue = Context.InstigatorComp->DefaultAttributes->GetFloatValue(AttributeMagnitude.BackingAttribute);
			break;
		case EGAAttributeSource::Target:
			AttributeValue = Context.TargetComp->DefaultAttributes->GetFloatValue(AttributeMagnitude.BackingAttribute);
			break;
		default:
			break;
		}
		switch (AttributeMagnitude.BackAttributeOperation)
		{
			case EGAMagnitudeOperation::Add:
				Result = Result + AttributeValue;
				break;
			case EGAMagnitudeOperation::Subtract:
				Result = Result - AttributeValue;
				break;
			case EGAMagnitudeOperation::Multiply:
				Result = Result * AttributeValue;
				break;
			case EGAMagnitudeOperation::Divide:
				Result = Result / AttributeValue;
				break;
			default:
				break;
		}
	}

	return Result;
}
float FGAAttributeSpec::GetCurrentMagnitude(const FGAEffectContext& Context)
{
	if (bUseMagnitude)
	{
		CalculatedMagnitude = CalcuclateMagnitude(Context);
		return CalculatedMagnitude;
	}
	else
	{
		CalculatedMagnitude = ModValue;
		return CalculatedMagnitude;
	}
	return 0;
}
void FGAAttributeModData::InitializeModData()
{
	//for (FGAAttributeSpec& spec : AttributeModSpec)
	//{
	//	//if (spec.bUseMagnitude)
	//	//{
	//		spec.AttributeMagnitude.AttributeContext = AttributeContext;
	//	//}
	//}
}

void FGAAttributeModData::CalculcateCurrentMods()
{
	//for (FGAAttributeSpec& spec : AttributeModSpec)
	//{
	//	spec.GetCurrentMagnitude();
	//}
}

FGAAttributeDataCallback FGAAttributeModData::ApplyMod()
{
	//if (!AttributeContext.Instigator.IsValid())
		return FGAAttributeDataCallback();
	//CalculcateCurrentMods();

	//return AttributeContext.Instigator->ModifyAttributesOnTarget(AttributeContext.Target.Get(), *this);
}

void FGAEffectInstant::CalculateMagnitude()
{
	for (FGAAttributeSpec& spec : AttributeData.AttributeModSpec)
	{
		spec.GetCurrentMagnitude(EffectContext);
	}
}
void FGAEffectInstant::ApplyInstantEffect()
{
//	CalculateMagnitude();
	if (bInstanceEffect)
	{
		EffectInstance = ConstructObject<UGAEffect>(EffectClass); //no outer, fire and forget.
	}
	//we still want to execute CDO, even if not object is explicitly instanced ?
	if (EffectClass)
	{
	//	EffectClass.GetDefaultObject()->OnEffectApplied(*this);
	}
	if (EffectInstance.IsValid())
	{
		EffectInstance->OnEffectInstanceApplied();
		EffectInstance->MarkPackageDirty(); //mark for GC.
		EffectInstance.Reset(); //reset pointer, we no longer need it any references.
	}

	GetInstigatorComp()->ModifyAttributesOnTarget(EffectContext, Handle, AttributeData);
}

void FGAEffectModiferSpec::ModifySpec(FGAAttributeSpec& SpecIn)
{
	float tempMag = 0;
	switch (ModType)
	{
	case EGAEffectModType::Add:
		//not ideal, I have to think of something better.
		tempMag = SpecIn.GetCalculatedMagnitude();
		tempMag += ModValue;
		SpecIn.SetCalculatedMagnitude(tempMag);
		break;
	case EGAEffectModType::Multiply:
		break;
	case EGAEffectModType::Subtract:
		break;
	case EGAEffectModType::Divide:
		break;
	case EGAEffectModType::PrecentageReduce:
		break;
	case EGAEffectModType::PrecentageIncrease:
		break;
	default:
		break;
	}
}

void FGAEffectDuration::RegisterDelegates(FGAOnPostEffectApplied& PostEffectApplied, FGAOnPostEffectRemoved& PostEffectRemoved)
{
	if (PostEffectApplied.IsBoundToObject(this) ||
		PostEffectRemoved.IsBoundToObject(this))
		return;

	PostEffectApplied.AddRaw(this, &FGAEffectDuration::OnPostEffectApplied);
	PostEffectRemoved.AddRaw(this, &FGAEffectDuration::OnPostEffectRemoved);
}

void FGAEffectDuration::OnPostEffectApplied(FGAEffectBase& EffectIn)
{
	//what happens if EffectIn is applied ?
	//should I do something about it ?
}
void FGAEffectDuration::OnPostEffectRemoved(FGAEffectBase& EffectIn)
{
	//what happens when EffectIn is removed ?
	//should I take any actions ?
}

//effect has been succeefully applied to target;
void FGAEffectDuration::OnApllied()
{
	if (bInstanceEffect)
	{
		EffectInstance = ConstructObject<UGAEffect>(EffectClass); //no outer, fire and forget.
	}
	//if (EffectClass)
	//{
	//	EffectClass.GetDefaultObject()->OnEffectApplied(*this);
	//}
	if (EffectInstance.IsValid())
	{
		EffectInstance->OnEffectInstanceApplied();
	}
}
//effect has continous effect(?) on target, like dot damage.
void FGAEffectDuration::OnContinious()
{
	if (EffectInstance.IsValid())
	{
		EffectInstance->OnEffectInstanceContinious();
	}
}
//effect has been prematurly removed by other offect or other means.
void FGAEffectDuration::OnRemoved()
{
	if (EffectInstance.IsValid())
	{
		EffectInstance->OnEffectInstanceRemoved();
		EffectInstance->MarkPackageDirty(); //mark for GC.
		EffectInstance.Reset(); //reset pointer, we no longer need it any references.
	}
}
//effect naturally expired (reached max duration).
void FGAEffectDuration::OnExpired()
{
	if (EffectInstance.IsValid())
	{
		EffectInstance->OnEffectInstanceExpired();
		EffectInstance->MarkPackageDirty(); //mark for GC.
		EffectInstance.Reset(); //reset pointer, we no longer need it any references.
	}
}

void FGAEffectDuration::ModifyAttributeSpec(FGAAttributeSpec& SpecIn)
{
	ModifierSpec.ModifySpec(SpecIn);
}

void FGAEffectDuration::ApplyEffect()
{
	for (FGAAttributeSpec& spec : AttributeSpec.OnAppliedSpec.AttributeModSpec)
	{
		spec.GetCurrentMagnitude(EffectContext);
	}
	FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAEffectDuration::EffecExpired);
	EffectContext.TargetComp->GetWorld()->GetTimerManager().SetTimer(DurationHandle, del, 10, false);

	GetInstigatorComp()->ModifyAttributesOnTarget(EffectContext, Handle, AttributeSpec.OnAppliedSpec);
}
void FGAEffectDuration::EffecExpired()
{
	EffectContext.TargetComp->GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	for (const FGAAttributeSpec& spec : AttributeSpec.OnAppliedSpec.AttributeModSpec)
	{
		FGAAttributeBase* attr = EffectContext.TargetComp->DefaultAttributes->GetAttribute(spec.Attribute);
		if (attr)
		{
			attr->RemoveBonus(Handle);
		}
	}

	//we need to notify attributes we have modified..
	EffectContext.TargetComp->EffectExpired(Handle);
	//EffectContext.TargetComp->ActiveEffects.Effects.Remove(Handle);
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


void FGAActiveEffectContainer::ApplyEffect(const FGAEffectContext& Ctx, FGAAttributeModData& SpecModifiers)
{
	for (FGAAttributeSpec& spec : SpecModifiers.AttributeModSpec)
	{
		spec.GetCurrentMagnitude(Ctx);
		float tempMag = 0;
		tempMag = spec.GetCalculatedMagnitude();
		tempMag += 10;
		spec.SetCalculatedMagnitude(tempMag);
	}
	/*
		So the idea is, that we don't really care from which effect, this modifier spec has come.
		At this point we only care what type of modifer it is (determined by tag).

		We then should find all effects which can somehow modify this spec (change magnitude)
		We let each effect process spec linearlly (in order from newest to oldest)

		After all modifications has been applied, we should apply this spec to target as normal.
	*/
}
