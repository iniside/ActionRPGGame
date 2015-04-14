// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
#include "Effects/GAEffectSpecification.h"
#include "GAEffectCue.h"
#include "GACalculation.h"

#include "GAEffects.h"

void FGAEffectModifier::RemoveMod(const FGAEffectHandle& HandleIn)
{
	IncomingModifiers.Remove(HandleIn);
	OutgoingModifiers.Remove(HandleIn);

	CalculateIncomingBonus();
	CalculateOutgoingBonus();
}

void FGAEffectModifier::CalculateIncomingBonus()
{
	IncomingStack.Additive = 0;
	IncomingStack.Subtractive = 0;
	IncomingStack.Multiply = 0;
	IncomingStack.Divide = 0;
	auto ModIt = IncomingModifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		for (const FGAModifier& mod : ModIt->Value)
		{
			switch (mod.AttributeMod)
			{
			case EGAAttributeMod::Add:
				IncomingStack.Additive += mod.Value;
				break;
			case EGAAttributeMod::Subtract:
				IncomingStack.Subtractive += mod.Value;
				break;
			case EGAAttributeMod::Multiply:
				IncomingStack.Multiply += mod.Value;
				break;
			case EGAAttributeMod::Divide:
				IncomingStack.Divide += mod.Value;
				break;
			default:
				break;
			}
		}
	}
}
void FGAEffectModifier::CalculateOutgoingBonus()
{
	OutgoingStack.Additive = 0;
	OutgoingStack.Subtractive = 0;
	OutgoingStack.Multiply = 0;
	OutgoingStack.Divide = 0;
	auto ModIt = OutgoingModifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		for (const FGAModifier& mod : ModIt->Value)
		{
			switch (mod.AttributeMod)
			{
			case EGAAttributeMod::Add:
				OutgoingStack.Additive += mod.Value;
				break;
			case EGAAttributeMod::Subtract:
				OutgoingStack.Subtractive += mod.Value;
				break;
			case EGAAttributeMod::Multiply:
				OutgoingStack.Multiply += mod.Value;
				break;
			case EGAAttributeMod::Divide:
				OutgoingStack.Divide += mod.Value;
				break;
			default:
				break;
			}
		}
	}
}
void FGAEffectModifier::RemoveWeakerBonus(EGAAttributeMod ModType, EGAModifierType EffectModType, float ValueIn)
{
	switch (EffectModType)
	{
	case EGAModifierType::Incoming:
		RemoveWeakerIncomingBonus(ModType, ValueIn);
		break;
	case EGAModifierType::Outgoing:
		RemoveWeakerOutgoingBonus(ModType, ValueIn);
		break;
	}
}
void FGAEffectModifier::RemoveWeakerIncomingBonus(EGAAttributeMod ModType, float ValueIn)
{
	for (auto aIt = IncomingModifiers.CreateIterator(); aIt; ++aIt)
	{
		for (auto It = aIt->Value.CreateIterator(); It; ++It)
		{
			if (It->AttributeMod == ModType
				&& It->Value < ValueIn)
			{
				aIt->Value.RemoveAt(It.GetIndex());
				if (aIt->Value.Num() <= 0)
				{
					aIt.RemoveCurrent();
				}
			}
		}
	}
	CalculateIncomingBonus();
}
void FGAEffectModifier::RemoveWeakerOutgoingBonus(EGAAttributeMod ModType, float ValueIn)
{
	for (auto aIt = OutgoingModifiers.CreateIterator(); aIt; ++aIt)
	{
		for (auto It = aIt->Value.CreateIterator(); It; ++It)
		{
			if (It->AttributeMod == ModType
				&& It->Value < ValueIn)
			{
				aIt->Value.RemoveAt(It.GetIndex());
				if (aIt->Value.Num() <= 0)
				{
					aIt.RemoveCurrent();
				}
			}
		}
	}
	CalculateOutgoingBonus();
}
void FGAEffectModifier::RemoveBonusOfType(EGAAttributeMod ModType, EGAModifierType EffectModType)
{
	switch (EffectModType)
	{
	case EGAModifierType::Incoming:
		RemoveIncomingBonusOfType(ModType);
		break;
	case EGAModifierType::Outgoing:
		RemoveOutgoingBonusOfType(ModType);
		break;
	}
}
void FGAEffectModifier::RemoveIncomingBonusOfType(EGAAttributeMod ModType)
{
	for (auto aIt = IncomingModifiers.CreateIterator(); aIt; ++aIt)
	{
		for (auto It = aIt->Value.CreateIterator(); It; ++It)
		{
			if (It->AttributeMod == ModType)
			{
				aIt->Value.RemoveAt(It.GetIndex());
				if (aIt->Value.Num() <= 0)
				{
					aIt.RemoveCurrent();
				}
			}
		}
	}
	CalculateIncomingBonus();
}
void FGAEffectModifier::RemoveOutgoingBonusOfType(EGAAttributeMod ModType)
{
	for (auto aIt = OutgoingModifiers.CreateIterator(); aIt; ++aIt)
	{
		for (auto It = aIt->Value.CreateIterator(); It; ++It)
		{
			if (It->AttributeMod == ModType)
			{
				aIt->Value.RemoveAt(It.GetIndex());
				if (aIt->Value.Num() <= 0)
				{
					aIt.RemoveCurrent();
				}
			}
		}
	}
	CalculateIncomingBonus();
}
void FGAEffectModifier::AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle)
{
	switch (ModifiersIn.ModifierType)
	{
	case EGAModifierType::Incoming:
		AddIncomingBonus(ModifiersIn, Handle);
		break;
	case EGAModifierType::Outgoing:
		AddOutgoingBonus(ModifiersIn, Handle);
		break;
	}
}
void FGAEffectModifier::AddIncomingBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle)
{
	TArray<FGAModifier>& modsTemp = IncomingModifiers.FindOrAdd(Handle);
	modsTemp.Add(ModifiersIn);
	CalculateIncomingBonus();
}
void FGAEffectModifier::AddOutgoingBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle)
{
	TArray<FGAModifier>& modsTemp = OutgoingModifiers.FindOrAdd(Handle);
	modsTemp.Add(ModifiersIn);
	CalculateOutgoingBonus();
}
void FGAEffectModifier::RemoveBonus(const FGAEffectHandle& Handle)
{
	//Modifiers.Remove(Handle);
	//CalculateBonus();
}

FGAEffectSpec::FGAEffectSpec(class UGAEffectSpecification* EffectSpecIn,
	const FGAEffectContext& ContextIn)
	: EffectSpec(EffectSpecIn),
	Context(ContextIn)
{
	Policy = EffectSpec->Policy;
	EffectDuration = EffectSpec->EffectDuration;

}

TArray<FGAAttributeData> FGAEffectSpec::GetInitialAttribute()
{
	return EffectSpec->GetInitialAttribute(Context);
}
TArray<FGAAttributeData> FGAEffectSpec::GetDurationAttribute()
{
	return EffectSpec->GetDurationAttribute(Context);
}
TArray<FGAAttributeData> FGAEffectSpec::GetPeriodAttribute()
{
	return EffectSpec->GetPeriodAttribute(Context);
}
TArray<FGAAttributeData> FGAEffectSpec::GetRemovedAttribute()
{
	return EffectSpec->GetRemovedAttribute(Context);
}
TArray<FGAAttributeData> FGAEffectSpec::GetExpiredAttribute()
{
	return EffectSpec->GetExpiredAttribute(Context);
}
TArray<FGAEffectModifierSpec> FGAEffectSpec::GetEffectModifiers()
{
	return EffectSpec->GetEffectModifiers(Context);
}

FGAEffectInstant::FGAEffectInstant(FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn)
{
	InitialAttribute = SpecIn.GetInitialAttribute();
	Context = ContextIn;
}
void FGAEffectInstant::OnApplied()
{

	FGAEffectHandle handle;
	for (const FGAAttributeData& data : InitialAttribute)
		Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, handle);
}

void FGAActiveDuration::RemoveDurationAttribute()
{

}

void FGAActiveDuration::OnApplied()
{
	TArray<FGAAttributeData> InitialModifiers = EffectSpec->GetInitialAttribute(Context);
	for (const FGAAttributeData& data : InitialModifiers)
	{
		//FGAAttributeData data = InitialAttribute;
		if (Context.InstigatorComp.IsValid())
			Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, MyHandle);
	}
	TArray<FGAAttributeData> AttributeModifiers = EffectSpec->GetDurationAttribute(Context);
	for (const FGAAttributeData& data : AttributeModifiers)
	{
		FGAAttributeBase* attr = Context.TargetComp->GetAttribute(data.Attribute);
		if (attr)
		{
			TSharedPtr<FGAActiveDuration> temp = AsShared();
			attr->AddBonus(FGAModifier(data.Mod, data.Value, temp), MyHandle);
		}
	}
}

void FGAActiveDuration::OnPeriod()
{
	if (!Context.TargetComp.IsValid() || !Context.InstigatorComp.IsValid())
		return;
	//recalculate spec on every tick. So we can get latest attributes, from Context
	//and by that if effect, uses them in calculation, we can always have correct result.
	//I will probabaly add option in future opt out of it
	TArray<FGAAttributeData> PeriodicModifiers = EffectSpec->GetPeriodAttribute(Context);
	for (const FGAAttributeData& data : PeriodicModifiers)
	{
		if (CalculationType)
		{
			FGAAttributeData DataIn = CalculationType.GetDefaultObject()->IncomingModifyEffect(data);
			//FGAAttributeData data = PeriodModifiers;
			if (Context.InstigatorComp.IsValid())
				Context.InstigatorComp->ModifyAttributesOnTarget(DataIn, Context, OwnedTags, MyHandle);
		}
	}
}
void FGAActiveDuration::OnRemoved()
{
	Context.TargetComp->EffectRemoved(MyHandle);
}
void FGAActiveDuration::OnEnded()
{
	Context.TargetComp->EffectExpired(MyHandle);
}
void FGAActiveDuration::StackDuration(float NewDuration)
{
	float remainingDuration = Context.Target->GetWorldTimerManager().GetTimerRemaining(DurationTimerHandle);
	StackedDuration = remainingDuration + NewDuration;

	Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	FTimerDelegate durationDel = FTimerDelegate::CreateRaw(this, &FGAActiveDuration::OnEnded);;
	Context.Target->GetWorldTimerManager().SetTimer(DurationTimerHandle, durationDel, StackedDuration, false);
}
void FGAActiveDuration::ActivateEffect()
{
	FTimerDelegate periodDel = FTimerDelegate::CreateRaw(this, &FGAActiveDuration::OnPeriod);
	Context.Target->GetWorldTimerManager().SetTimer(PeriodTimerHandle, periodDel, Period, true);

	FTimerDelegate durationDel = FTimerDelegate::CreateRaw(this, &FGAActiveDuration::OnEnded);;
	Context.Target->GetWorldTimerManager().SetTimer(DurationTimerHandle, durationDel, Duration, false);

	OnApplied();
}
void FGAActiveDuration::FinishEffect()
{
	if (Context.Target.IsValid())
	{
		Context.Target->GetWorldTimerManager().ClearTimer(PeriodTimerHandle);
		Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	}
	TArray<FGAAttributeData> DurationModifiers = EffectSpec->GetDurationAttribute(Context);
	if (Context.TargetComp.IsValid())
	{
		for (const FGAAttributeData& data : DurationModifiers)
		{
			FGAAttributeBase* attr = Context.TargetComp->GetAttribute(data.Attribute);
			if (attr)
			{
				attr->RemoveBonus(MyHandle);
			}
		}
	}
}
bool FGAActiveDuration::ComparePeriodModifiers(const FGAAttributeData& OtherIn)
{
	return false;
}
FGAActiveDuration::FGAActiveDuration(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
	const FGAEffectHandle& HandleIn)
	//	: Context(ContextIn)
{
	EffectSpec = SpecIn.EffectSpec;
	AggregationType = SpecIn.Policy.Aggregation;
	Stacking = SpecIn.Policy.Stacking;

	EffectName = SpecIn.EffectName;
	Duration = SpecIn.EffectDuration.Duration;
	Period = SpecIn.EffectDuration.Period;
	MyHandle = HandleIn;
	Context = ContextIn;
	CalculationType = SpecIn.EffectSpec->CalculationType;
	//OwnedTags = SpecIn.EffectTags;

	RemovedAttribute = SpecIn.GetRemovedAttribute();
	ExpiredAttribute = SpecIn.GetExpiredAttribute();
}

FGAActiveDuration::~FGAActiveDuration()
{
	FinishEffect();
	Context.Reset();
}

FGAActiveEffect::FGAActiveEffect(const FGAEffectHandle& HandleIn, FGAEffectSpec& SpecIn, float StartTimeIn)
	: MyHandle(HandleIn),
	Duration(SpecIn.EffectDuration.Duration),
	Context(SpecIn.Context),
	WorldStartTime(StartTimeIn)
{
	bIsActivated = false;
}

FGAActiveEffect::FGAActiveEffect(const FGAEffectHandle& HandleIn, FGAEffectSpec& SpecIn, float StartTimeIn,
	TSubclassOf<class UGAUIData> UIDataIn, TSubclassOf<class AGAEffectCue> CueIn)
	: MyHandle(HandleIn),
	Duration(SpecIn.EffectDuration.Duration),
	Context(SpecIn.Context),
	WorldStartTime(StartTimeIn),
	UIDataClass(UIDataIn),
	CueClass(CueIn)
{
	bIsActivated = false;
}

float FGAActiveEffect::GetRemainingDuration(float CurrentWorldTime)
{
	return Duration - (CurrentWorldTime - WorldStartTime);
}
void FGAEffectModifierContainer::RemoveModifier(const FGameplayTagContainer& TagsIn, const FGAEffectHandle& HandleIn)
{
	for (const FGameplayTag& tag : TagsIn)
	{
		FGAEffectModifier* mods = Modifiers.Find(tag);
		if (mods)
			mods->RemoveMod(HandleIn);
	}
}
void FGAEffectModifierContainer::RemoveWeakerModifiers(const FGameplayTagContainer& TagsIn, 
	const TArray<FGAEffectModifierSpec>& ModSpec)
{
	for (const FGameplayTag& Tag : TagsIn)
	{
		FGAEffectModifier* EffMod = Modifiers.Find(Tag);
		if (!EffMod)
			continue;

		for (const FGAEffectModifierSpec spec : ModSpec)
		{
			EffMod->RemoveWeakerBonus(spec.Mod, spec.ModifierType, spec.DirectModifier.Value);
		}
	}
}
void FGAEffectModifierContainer::RemoveModifiersByType(const FGameplayTagContainer& TagsIn, 
	const TArray<FGAEffectModifierSpec>& ModSpec)
{
	for (const FGameplayTag& Tag : TagsIn)
	{
		FGAEffectModifier* EffMod = Modifiers.Find(Tag);
		if (!EffMod)
			continue;

		for (const FGAEffectModifierSpec spec : ModSpec)
		{
			EffMod->RemoveBonusOfType(spec.Mod, spec.ModifierType);
		}
	}
}
void FGAEffectModifierContainer::AddModifier(const FGAEffectModifierSpec& ModSpec, const FGameplayTagContainer& Tags,
	const FGAEffectHandle HandleIn,
	TSharedPtr<FGAActiveDuration> EffectPtr)
{
	FString complexString = Tags.ToString();
	FString simpleString = Tags.ToStringSimple();
	FName test(*simpleString);

	for (const FGameplayTag& tag : Tags)
	{
		FGAEffectModifier& mods = Modifiers.FindOrAdd(tag);

		FGAModifier modifier(ModSpec.Mod, ModSpec.DirectModifier.Value, EffectPtr);
		modifier.ModifierType = ModSpec.ModifierType;
		mods.AddBonus(modifier, HandleIn);
	}
}
void FGAEffectModifierContainer::CheckIfStronger(const FGameplayTagContainer& TagsIn, TArray<FGAEffectModifierSpec>& ModSpecs)
{
	for (const FGameplayTag& tag : TagsIn)
	{
		FGAEffectModifier* mod = Modifiers.Find(tag);
		if (mod)
		{
		}
	}
}
FGAModifierStack FGAEffectModifierContainer::GetIncomingModifierStack(const FGAAttributeData& DataIn)
{
	FGAModifierStack StackOut;
	for (const FGameplayTag& DataTag : DataIn.AgreggatedTags)
	{
		FGAEffectModifier* mod = Modifiers.Find(DataTag);
		if (mod)
		{
			StackOut.Add(mod->IncomingStack);
		}
	}
	return StackOut;
}
FGAModifierStack FGAEffectModifierContainer::GetOutgoingModifierStack(const FGAAttributeData& DataIn)
{
	FGAModifierStack StackOut;
	for (const FGameplayTag& DataTag : DataIn.AgreggatedTags)
	{
		FGAEffectModifier* mod = Modifiers.Find(DataTag);
		if (mod)
		{
			StackOut.Add(mod->OutgoingStack);
		}
	}
	return StackOut;
}
FGAEffectHandle FGAActiveEffectContainer::ApplyEffect(TSubclassOf<class UGAEffectSpecification> SpecIn,
	const FGAEffectContext& Ctx, const FName& EffectName)
{
	if (!SpecIn)
		return FGAEffectHandle();

	FGAEffectSpec spec(SpecIn.GetDefaultObject(), Ctx);
	spec.EffectName.EffectName = EffectName;
	switch (spec.Policy.Type)
	{
	case EGAEffectType::Instant:
	{
		FGAEffectInstant instntEffect(spec, Ctx);
		return HandleInstantEffect(instntEffect, Ctx);
	}
	case EGAEffectType::Periodic:
	{
		return HandleDurationEffect(spec, Ctx);
	}
	case EGAEffectType::Duration:
	{
		return HandleDurationEffect(spec, Ctx);
	}
	case EGAEffectType::Infinite:
	{
		break;
	}
	default:
	{
		return FGAEffectHandle();
	}
	}
	return FGAEffectHandle();
}


void FGAActiveEffectContainer::RemoveActiveEffect(const FGAEffectHandle& HandleIn)
{
	for (auto It = RepActiveEffects.CreateIterator(); It; ++It)
	{
		if (It->MyHandle == HandleIn)
		{
			RepActiveEffects.RemoveAt(It.GetIndex());
			break;
		}
	}

	TSharedPtr<FGAActiveDuration> removedEffect;
	ActiveEffects.RemoveAndCopyValue(HandleIn, removedEffect);
	if (removedEffect.IsValid())
	{
		ModifierContainer.RemoveModifier(removedEffect->EffectSpec->RequiredTags, HandleIn);
		switch (removedEffect->AggregationType)
		{
		case EGAEffectAggregation::AggregateByInstigator:
			RemoveInstigatorAggregation(removedEffect);
			break;
		case EGAEffectAggregation::AggregateByTarget:
			RemoveTargetAggregation(removedEffect);
			break;
		}

		/*
			Clear modifiers, applied directly to attributes (if any).
			*/
		removedEffect->FinishEffect();
		removedEffect.Reset();
	}
}
void FGAActiveEffectContainer::RemoveTargetAggregation(TSharedPtr<FGAActiveDuration> EffectIn)
{
	TArray<FGAEffectHandle>* handles;
	handles = MyEffects.Find(EffectIn->EffectName);

	for (auto It = handles->CreateIterator(); It; ++It)
	{
		if (*It == EffectIn->MyHandle)
		{
			handles->RemoveAtSwap(It.GetIndex());
		}
	}
}
void FGAActiveEffectContainer::RemoveInstigatorAggregation(TSharedPtr<FGAActiveDuration> EffectIn)
{
	FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(EffectIn->Context.InstigatorComp);

	for (auto It = instCont.Effects.CreateIterator(); It; ++It)
	{
		if (It->Handle == EffectIn->MyHandle)
		{
			instCont.Effects.RemoveAtSwap(It.GetIndex());
		}
	}

}
FGAEffectHandle FGAActiveEffectContainer::AddActiveEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle handle = FGAEffectHandle::GenerateHandle();
	TSharedPtr<FGAActiveDuration> tempPeriodic = MakeShareable(new FGAActiveDuration(Ctx, EffectIn, handle));
	tempPeriodic->ActivateEffect();

	FGAActiveEffect activeEffect(handle, EffectIn, Ctx.Target->GetWorld()->GetTimeSeconds(),
		EffectIn.EffectSpec->UIData, EffectIn.EffectSpec->EffectCue);

	activeEffect.Duration = EffectIn.EffectSpec->EffectDuration.Duration;
	activeEffect.Period = EffectIn.EffectSpec->EffectDuration.Period;

	RepActiveEffects.Add(activeEffect);

	for (const FGAEffectModifierSpec& ad : EffectIn.GetEffectModifiers())
	{
		ModifierContainer.AddModifier(ad, EffectIn.EffectSpec->RequiredTags, handle, tempPeriodic);
	}
	ActiveEffects.Add(handle, tempPeriodic);

	return handle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx)
{
	if (Ctx.TargetComp.IsValid() && Ctx.InstigatorComp.IsValid())
	{
		FGAEffectHandle handle;
		SpecIn.OnApplied();
	}
	return FGAEffectHandle();
}
FGAEffectHandle FGAActiveEffectContainer::HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	//TODO::
	//1. Comment this.
	//2. Handle stacking.
	//3. Handle conditional effect applying (if have this tags, apply this effect)
	//4. effect apply effect. Not really important for Instant effect, but Duration based, have 
	//   stages at which additonal effects can be applied, and only those effects can know when to do it.
	//5. Add support for UObject effect extensions which are instanced per application, for special logic.

	/*
		Determine against whom we should check stacking rules.
		Idea is, if effect is grouped by Instigator, stacking rules are checked ONLY
		against other effects from the same instigator, and never checked against anything else.
		If effect is grouped by target, stacking rules, are checked ONLY against other effects, grouped
		by target.

		The two of them never talk to each other, and never check for each other effects.
		So you must be careful, when setting up effects, and stacking rules!.

		A good example, would Bleed Condition effect, and simple Corruption spell (which just deals damage over time).
		Bleed condition, would be aggregated by target. So who applied it wouldn't matter.
		Stacking rules, would be check for all existing bleed condition on target. So for example,
		we can set that there can only be ever ONE bleed condition on target, and strongest one
		will always override anything that is on target.

		Corruption would be stacked by Instigator. This means, that if two players attack one enemy, and
		they both apply Corruption to it, target will have TWO Corruption effects applied.
		Neither Corruption effect, cares, about the other. If one  player apply Corruption again,
		it will just check stacking rules against corruption, applied by the same player.

		If you want to make non stackable buffs (like +50hp, +100 mana etc), you should always aggregate
		aggregate them by Target.
		If you aggregate them by Instigator, various buffs, from various instigator will stack, togather.
		If one player will apply +50hp and other +100hp, in this case you will end up with +150HP buff.

		If you group by target, and set to Highest Override, only +100HP buff will be applied, and all
		others will be removed from target.
		*/
	switch (EffectIn.EffectSpec->Policy.Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		return HandleInstigatorAggregationEffect(EffectIn, Ctx);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		return HandleTargetAggregationEffect(EffectIn, Ctx);
		break;
	}
	default:
		break;
	}

	//we got to this point, it should be safe to generate handle.

	return FGAEffectHandle();
}

FGAEffectHandle FGAActiveEffectContainer::HandleInstigatorAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	/*
		1. StrongerOverride - if incoming effect of the same type as existing one (have the same
		EffectIn.MyTag), is stronger than existing one it will override it.
		Other wise existing effect, will remain unchanged on target.
		2. Override - Effect will simplt override existing effect on the same type.
		3. Duration - will take remaning duration of existing effect, and add it's duration to stack.
		Nothing else is changed.
		4. Inensity - Will add existing attribute modifiers, with the new one.
		Technically it might look like StackCount*Magnitude.
		5. Add - Adds new effect to stack. Check for nothing, and change nothing.
		*/
	switch (EffectIn.EffectSpec->Policy.Stacking)
	{
	case EGAEffectStacking::StrongerOverride:
	{
		HandleInstigatorEffectStrongerOverride(EffectIn, Ctx);
		break;
	}
	case EGAEffectStacking::Override:
	{
		return HandleInstigatorEffectOverride(EffectIn, Ctx);
		break;
	}
	case EGAEffectStacking::Duration:
	{
		return HandleInstigatorEffectDuration(EffectIn, Ctx);
		break;
	}
	case EGAEffectStacking::Intensity:
	{
		break;
	}
	case EGAEffectStacking::Add:
	{
		return HandleInstigatorEffectAdd(EffectIn, Ctx);
		break;
	}
	default:
		break;
	}
	return FGAEffectHandle();
}
FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	/*
		To Consider:
		1. Current implementation does not check for tags or consider other effects.
		When FGAAttributeBase attribute is modified by DurationAttribute, we just check
		if incoming attribute have the same mode type and have higher value.
		If it does, we simply remove old modifier, and apply new one.
		Should we check for tags ? (in this case we would need to modify, FGAModifer, to contains
		info about tag, or pointer to effect, which applied, from which we could pull tags).
		Should we check for effect name ? Ie. only the same effect type can override attributes
		(that's kind of pointless, since same effect, should just override it's older copy).


		What for other attributes attributes ?
		InitialAttribute is not important.
		PeriodAttribute, RemovedAttribute,  ExpiredAttribute
		- thos modify only, primitive attributes (floats)
		*/
	FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(Ctx.InstigatorComp);
	FGAEffectHandle foundHandle;


	for (FGAEffectTagHandle& eff : instCont.Effects)
	{
		if (eff.EffectName == EffectIn.EffectName)
		{
			foundHandle = eff.Handle;
			break;
		}
	}

	/*
		If stacking is StrongerOverride, we first check if attribute is already modified by anything.
		If it is, we remove any applicable mods which are weaker than ours.

		We do not make any checks for tags or other effects, They are of no concern to us, when it comes
		to modifing complex attribute.
	*/

	TArray<FGAAttributeData> AttributeModifiers = EffectIn.EffectSpec->GetDurationAttribute(Ctx);
	for (const FGAAttributeData& data : AttributeModifiers)
	{
		FGAAttributeBase* AtrPtr = Ctx.TargetComp->GetAttribute(data.Attribute);
		if (AtrPtr)
		{
			AtrPtr->RemoveWeakerBonus(data.Mod, data.Value);
		}
	}

	ModifierContainer.RemoveWeakerModifiers(EffectIn.EffectSpec->RequiredTags, EffectIn.EffectSpec->EffectModifiers);

	RemoveActiveEffect(foundHandle);
	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);

	FGAEffectTagHandle nameHandle(EffectIn.EffectName, handle);
	instCont.Effects.Add(nameHandle);
	return handle;
}
FGAEffectHandle FGAActiveEffectContainer::HandleInstigatorEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(Ctx.InstigatorComp);
	FGAEffectHandle foundHandle;// = instCont.EffectsByName.FindRef(EffectIn.EffectName);
	for (FGAEffectTagHandle& eff : instCont.Effects)
	{
		if (eff.EffectName == EffectIn.EffectName)
		{
			foundHandle = eff.Handle;
			break;
		}
	}
	/*
		1. If effect is set to override should:
		a). Remove all attribute modifiers, which are the same as ours ?
		b). Or should we just override modifiers, applied by the same effect (identified, by effects name/handle).
	*/
	TArray<FGAAttributeData> AttributeModifiers = EffectIn.EffectSpec->GetDurationAttribute(Ctx);
	for (const FGAAttributeData& data : AttributeModifiers)
	{
		FGAAttributeBase* AtrPtr = Ctx.TargetComp->GetAttribute(data.Attribute);
		if (AtrPtr)
		{
			AtrPtr->RemoveBonusByType(data.Mod);
		}
	}

	RemoveActiveEffect(foundHandle);

	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);
	FGAEffectTagHandle nameHandle(EffectIn.EffectName, handle);
	instCont.Effects.Add(nameHandle);

	return handle;
}
FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle foundHandle;
	FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(Ctx.InstigatorComp);

	for (FGAEffectTagHandle& eff : instCont.Effects)
	{
		if (EffectIn.EffectName == eff.EffectName)
		{
			foundHandle = eff.Handle;
			break;
		}
	}

	if (foundHandle.IsValid())
	{
		TSharedPtr<FGAActiveDuration> durationEffect = ActiveEffects.FindRef(foundHandle);
		if (durationEffect.IsValid())
		{
			durationEffect->StackDuration(EffectIn.EffectDuration.Duration);
		}
	}
	else
	{
		//if handle is not valid, it means there is no effect,
		//and this means we have to add new effect.
		foundHandle = AddActiveEffect(EffectIn, Ctx);
		instCont.Effects.Add(FGAEffectTagHandle(EffectIn.EffectName, foundHandle));
	}

	return foundHandle;
}
FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectDIntensity(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	return FGAEffectHandle();
}

FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);
	return handle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleTargetAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	switch (EffectIn.Policy.Stacking)
	{
	case EGAEffectStacking::StrongerOverride:
	{
		return HandleTargetEffectStrongerOverride(EffectIn, Ctx);
	}
	case EGAEffectStacking::Override:
	{
		return HandleTargetEffectOverride(EffectIn, Ctx);
	}
	case EGAEffectStacking::Duration:
	{
		break;
	}
	case EGAEffectStacking::Intensity:
	{
		return HandleTargetEffectDuration(EffectIn, Ctx);
	}
	case EGAEffectStacking::Add:
	{
		return HandleTargetEffectAdd(EffectIn, Ctx);
	}
	default:
		break;
	}
	return FGAEffectHandle();
}
FGAEffectHandle	FGAActiveEffectContainer::HandleTargetEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	TArray<FGAEffectHandle> handles;
	handles = MyEffects.FindRef(EffectIn.EffectName);

	for (const FGAEffectHandle& hand : handles)
	{
		RemoveActiveEffect(hand);
	}

	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);

	TArray<FGAEffectHandle>& addedHandle = MyEffects.FindOrAdd(EffectIn.EffectName);
	addedHandle.Add(handle);

	return handle;
}
FGAEffectHandle FGAActiveEffectContainer::HandleTargetEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	TArray<FGAEffectHandle> handles;
	handles = MyEffects.FindRef(EffectIn.EffectName);

	for (const FGAEffectHandle& hand : handles)
	{
		RemoveActiveEffect(hand);
	}

	FGAEffectHandle newHandle = AddActiveEffect(EffectIn, Ctx);

	TArray<FGAEffectHandle>& addedHandle = MyEffects.FindOrAdd(EffectIn.EffectName);
	addedHandle.Add(newHandle);

	return newHandle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleTargetEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	TArray<FGAEffectHandle> handles;
	handles = MyEffects.FindRef(EffectIn.EffectName);

	for (FGAEffectHandle& hand : handles)
	{
		if (hand.IsValid())
		{
			TSharedPtr<FGAActiveDuration> durationEffect = ActiveEffects.FindRef(hand);
			if (durationEffect.IsValid())
			{
				durationEffect->StackDuration(EffectIn.EffectDuration.Duration);
			}
		}
	}
	FGAEffectHandle returnHandle;
	if (handles.Num() == 0)
	{
		//if handle is not valid, it means there is no effect,
		//and this means we have to add new effect.
		returnHandle = AddActiveEffect(EffectIn, Ctx);
		handles.Add(returnHandle);
		MyEffects.Add(EffectIn.EffectName, handles);
	}
	return returnHandle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleTargetEffectIntensity(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	return FGAEffectHandle();
}

FGAEffectHandle FGAActiveEffectContainer::HandleTargetEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle newHandle = AddActiveEffect(EffectIn, Ctx);

	TArray<FGAEffectHandle>& addedHandle = MyEffects.FindOrAdd(EffectIn.EffectName);
	addedHandle.Add(newHandle);

	return newHandle;
}

void FGAActiveEffectContainer::Clean()
{
	ActiveEffects.Empty();
}