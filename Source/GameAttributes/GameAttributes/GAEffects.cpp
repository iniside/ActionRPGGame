// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
#include "Effects/GAEffectSpecification.h"
#include "GAEffects.h"


TArray<FGAAttributeData> FGAEffectSpec::GetInitialAttribute()
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.InitialAttributes)
	{
		modsOut.Add(mod.GetModifier(Context));
	}
	return modsOut;
}
TArray<FGAAttributeData> FGAEffectSpec::GetDurationAttribute()
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.DurationAttributes)
	{
		modsOut.Add(mod.GetModifier(Context));
	}
	return modsOut;
}
TArray<FGAAttributeData> FGAEffectSpec::GetPeriodAttribute()
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.PeriodAttributes)
	{
		modsOut.Add(mod.GetModifier(Context));
	}
	return modsOut;
}
TArray<FGAAttributeData> FGAEffectSpec::GetRemovedAttribute()
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.RemovedAttributes)
	{
		modsOut.Add(mod.GetModifier(Context));
	}
	return modsOut;
}
TArray<FGAAttributeData> FGAEffectSpec::GetExpiredAttribute()
{
	TArray<FGAAttributeData> modsOut;
	for (FGAAttributeModifier& mod : AttributeSpec.ExpiredAttributes)
	{
		modsOut.Add(mod.GetModifier(Context));
	}
	return modsOut;
}


FGAEffectInstant::FGAEffectInstant(FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn)
{
	InitialAttribute = SpecIn.GetInitialAttribute();
}

void FGAActiveDuration::RemoveDurationAttribute()
{

}

void FGAActiveDuration::OnApplied()
{
	for (FGAAttributeData data : PeriodModifiers)
	{
		//FGAAttributeData data = InitialAttribute;
		if (Context.InstigatorComp.IsValid())
			Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, MyHandle);
	}	
	/*
		If stacking is StrongerOverride, we first check if attribute is already modified by anything.
		If it is, we remove any applicable mods which are weaker than ours.

		We do not make any checks for tags or other effects, They are of no concern to us, when it comes
		to modifing complex attribute.
	*/
	if (Stacking == EGAEffectStacking::StrongerOverride)
	{
		for (const FGAAttributeData& data : DurationAttribute)
		{
			FGAAttributeBase* AtrPtr = Context.TargetComp->GetAttribute(data.Attribute);
			if (AtrPtr)
			{
				AtrPtr->RemoveWeakerBonus(data.Mod, data.Value);
			}
		}
	}

	for (const FGAAttributeData& data : DurationAttribute)
	{
		FGAAttributeBase* attr = Context.TargetComp->GetAttribute(data.Attribute);
		if (attr)
		{
			attr->AddBonus(FGAModifier(data.Mod, data.Value, MakeShareable(this)), MyHandle);
		}
	}
}

void FGAActiveDuration::OnPeriod()
{
	//delibetry copy, not reference!
	//if we would make reference original attribute data would be modified.
	//if const reference, then effects couldn't be modified down in the chain.
	//and so we end up with copy.
	for (FGAAttributeData data : PeriodModifiers)
	{
		//FGAAttributeData data = PeriodModifiers;
		if (Context.InstigatorComp.IsValid())
			Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, MyHandle);
	}
}
void FGAActiveDuration::OnRemoved()
{

}
void FGAActiveDuration::OnEnded()
{
	Context.TargetComp->EffectExpired(MyHandle);
}
void FGAActiveDuration::RestartTimer(float NewDuration)
{
	Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	FTimerDelegate durationDel = FTimerDelegate::CreateRaw(this, &FGAActiveDuration::OnEnded);;
	Context.Target->GetWorldTimerManager().SetTimer(DurationTimerHandle, durationDel, NewDuration, false);
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

	if (Context.TargetComp.IsValid())
	{
		for (const FGAAttributeData& data : PeriodModifiers)
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
	AggregationType = SpecIn.Policy.Aggregation;
	Stacking = SpecIn.Policy.Stacking;
	EffectName = SpecIn.EffectName;
	Duration = SpecIn.EffectDuration.Duration;
	Period = SpecIn.EffectDuration.Period;
	MyHandle = HandleIn;
	Context = ContextIn;
	//OwnedTags = SpecIn.EffectTags;
	InitialAttribute = SpecIn.GetInitialAttribute();
	DurationAttribute = SpecIn.GetDurationAttribute();
	PeriodModifiers = SpecIn.GetPeriodAttribute(); //we probabaly want to recalculate it on every tick
	////if effect.
	RemovedAttribute = SpecIn.GetRemovedAttribute();
	ExpiredAttribute = SpecIn.GetExpiredAttribute();
}

FGAActiveDuration::~FGAActiveDuration()
{
	FinishEffect();
	Context.Reset();
}

float FGAEffectModifier::GetFinalValue()
{
	switch (CalculationType)
	{
	case EGAMagnitudeCalculation::Direct:
		return DirectModifier.GetValue();
	case EGAMagnitudeCalculation::AttributeBased:
		break;
	case EGAMagnitudeCalculation::CurveBased:
		break;
	case EGAMagnitudeCalculation::CustomCalculation:
		break;
	case EGAMagnitudeCalculation::Invalid:
		break;
	default:
		break;
	}
	return 0;
}

FGAEffectHandle FGAActiveEffectContainer::ApplyEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Ctx)
{
	switch (SpecIn.Policy.Type)
	{
	case EGAEffectType::Instant:
	{
		FGAEffectInstant instntEffect;// (SpecIn, Ctx);
		return HandleInstantEffect(instntEffect, Ctx);
	}
	case EGAEffectType::Periodic:
	{	
		FGAEffectSpec& nonConst = const_cast<FGAEffectSpec&>(SpecIn);
		return HandleDurationEffect(nonConst, Ctx);
	}
	case EGAEffectType::Duration:
	{
		FGAEffectSpec& nonConst = const_cast<FGAEffectSpec&>(SpecIn);
		return HandleDurationEffect(nonConst, Ctx);
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

void FGAActiveEffectContainer::ExecuteEffectModifier(FGAAttributeData& ModifierIn,
	const FGameplayTagContainer& EffectTags, const FGAEffectContext& Ctx)
{
}
void FGAActiveEffectContainer::RemoveActiveEffect(const FGAEffectHandle& HandleIn)
{
	TSharedPtr<FGAActiveDuration> removedEffect;
	ActiveEffects.RemoveAndCopyValue(HandleIn, removedEffect);
	if (removedEffect.IsValid())
	{
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
	ActiveEffects.Add(handle, tempPeriodic);

	return handle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx)
{
	if (Ctx.TargetComp.IsValid() && Ctx.InstigatorComp.IsValid())
	{
		FGAEffectHandle handle;
		//Ctx.InstigatorComp->ModifyAttributesOnTarget(SpecIn.Modifiers, Ctx, handle);
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
	switch (EffectIn.Policy.Aggregation)
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
	switch (EffectIn.Policy.Stacking)
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

	RemoveActiveEffect(foundHandle);
	FGAEffectHandle handle;// = AddActiveEffect(EffectIn, Ctx);

	//FGAEffectTagHandle nameHandle(EffectIn.EffectName, handle);
	//instCont.Effects.Add(nameHandle);

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

	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);
	FGAEffectTagHandle nameHandle(EffectIn.EffectName, handle);
	instCont.Effects.Add(nameHandle);
	
	return handle;
}
FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);
	return handle;
}
FGAEffectHandle	FGAActiveEffectContainer::HandleInstigatorEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle foundHandle;
	//FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(Ctx.InstigatorComp);

	//for (FGAEffectTagHandle& eff : instCont.Effects)
	//{
	//	if (EffectIn.EffectName == eff.EffectName)
	//	{
	//		foundHandle = eff.Handle;
	//		break;
	//	}
	//}
	//
	//if (foundHandle.IsValid())
	//{
	//	TSharedPtr<FGAActiveDuration> durationEffect = ActiveEffects.FindRef(foundHandle);
	//	if (durationEffect.IsValid())
	//	{
	//		durationEffect->RestartTimer(EffectIn.EffectDuration.Duration);
	//	}
	//}
	//else
	//{
	//	//if handle is not valid, it means there is no effect,
	//	//and this means we have to add new effect.
	//	foundHandle = AddActiveEffect(EffectIn, Ctx);
	//	instCont.Effects.Add(FGAEffectTagHandle(EffectIn.EffectName, foundHandle));
	//}
	
	return foundHandle;
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
		break;
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
	//TArray<FGAEffectHandle> handles;
	//handles = MyEffects.FindRef(EffectIn.EffectName);

	//for (const FGAEffectHandle& hand : handles)
	//{
	//	RemoveActiveEffect(hand);
	//}

	FGAEffectHandle newHandle;// = AddActiveEffect(EffectIn, Ctx);

	//TArray<FGAEffectHandle>& addedHandle = MyEffects.FindOrAdd(EffectIn.EffectName);
	//addedHandle.Add(newHandle);

	return newHandle;
}

FGAEffectHandle FGAActiveEffectContainer::HandleTargetEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle newHandle;// = AddActiveEffect(EffectIn, Ctx);

	//TArray<FGAEffectHandle>& addedHandle = MyEffects.FindOrAdd(EffectIn.EffectName);
	//addedHandle.Add(newHandle);

	return newHandle;
}

void FGAActiveEffectContainer::Clean()
{

	ActiveEffects.Empty();
}