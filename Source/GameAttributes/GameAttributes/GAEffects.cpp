// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
#include "Effects/GAEffect.h"
#include "Effects/GAEffectSpecification.h"
#include "GAEffects.h"


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
TArray<FGAAttributeData> FGAEffectSpec::GetModifiers()
{
	TArray<FGAAttributeData> temp;
	for (FGAAttributeModifier& mod : AttributeModifiers)
	{
		temp.Add(mod.GetModifier(Context));
	}
	return temp;
}
TArray<FGAAttributeData> FGAEffectSpec::GetAttributeModifiers()
{
	TArray<FGAAttributeData> temp;
	for (FGAAttributeModifier& mod : AttributeModifiers)
	{
		temp.Add(mod.GetModifier(Context));
	}
	return temp;
}
TArray<FGAAttributeData> FGAEffectSpec::GetPeriodModifiers()
{
	for (FGAAttributeModifier& mod : PeriodModifiers)
	{
		PeriodMods.Add(mod.GetModifier(Context));
	}
	return PeriodMods;
}
TArray<FGAAttributeData> FGAEffectSpec::GetOnEndedModifiers()
{
	TArray<FGAAttributeData> returnValue;
	for (FGAAttributeModifier& mod : ExpiredModifiers)
	{
		returnValue.Add(mod.GetModifier(Context));
	}
	return returnValue;
}
TArray<FGAAttributeData> FGAEffectSpec::GetOnRemovedModifiers()
{
	TArray<FGAAttributeData> returnValue;
	for (FGAAttributeModifier& mod : RemovedModifiers)
	{
		returnValue.Add(mod.GetModifier(Context));
	}
	return returnValue;
}
void FGAActiveBase::OnApplied()
{
	TArray<FGAAttributeData> data = OnAppliedModifiers;
	if (Context.InstigatorComp.IsValid())
		Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, MyHandle);
}
FGAEffectInstant::FGAEffectInstant(const FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn)
{
	OnAppliedModifiers = SpecIn.EvalModifiers;
}
void FGAActiveDuration::OnPeriod()
{
	TArray<FGAAttributeData> data = PeriodModifiers;
	if (Context.InstigatorComp.IsValid())
		Context.InstigatorComp->ModifyAttributesOnTarget(data, Context, OwnedTags, MyHandle);
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
		AttributeModifiers.Empty();
		OnAppliedModifiers.Empty();
		OnEndedModifiers.Empty();
		OnRemovedModifiers.Empty();
		PeriodModifiers.Empty();
		Context.Target->GetWorldTimerManager().ClearTimer(PeriodTimerHandle);
		Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	}
}
bool FGAActiveDuration::ComparePeriodModifiers(const FGAAttributeData& OtherIn)
{
	TArray<FGAAttributeData> allMods;
	allMods.Append(OnEndedModifiers);
	allMods.Append(OnRemovedModifiers);
	allMods.Append(PeriodModifiers);
	for (const FGAAttributeData& data : allMods)
	{
		if (OtherIn > data)
			return true;
	}
	return false;
}
FGAActiveDuration::FGAActiveDuration(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
	const FGAEffectHandle& HandleIn)
	//	: Context(ContextIn)
{
	AggregationType = SpecIn.Policy.Aggregation;
	EffectName = SpecIn.EffectName;
	Duration = SpecIn.EffectDuration.Duration;
	Period = SpecIn.EffectDuration.Period;
	MyHandle = HandleIn;
	Context = ContextIn;
	OwnedTags = SpecIn.EffectTags;
	AttributeModifiers = SpecIn.GetAttributeModifiers();
	PeriodModifiers = SpecIn.GetPeriodModifiers(); //we probabaly want to recalculate it on every tick
	//if effect.
	OnAppliedModifiers = SpecIn.GetModifiers();
	OnEndedModifiers = SpecIn.GetOnEndedModifiers();
	OnRemovedModifiers = SpecIn.GetOnRemovedModifiers();
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
		FGAEffectInstant instntEffect(SpecIn, Ctx);
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
		for (FGAAttributeData& data : removedEffect->AttributeModifiers)
		{
			FGAAttributeBase* attr = removedEffect->Context.TargetComp->GetAttribute(data.Attribute);
			if (attr)
			{
				attr->RemoveBonus(HandleIn);
			}
		}
		

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

	for (FGAAttributeData& data : EffectIn.GetAttributeModifiers())
	{
		FGAAttributeBase* attr = Ctx.TargetComp->GetAttribute(data.Attribute);
		if (attr)
		{
			attr->AddBonus(FGAModifier(data.Mod, data.Value), handle);
		}
	}

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
	// stages at which additonal effects can be applied, and only those effects can know when to do it.
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
		Highest override, should override all other effects,
		which modify the same attribute (AttributeTag).

		If we have Effect which modify Damage.Fire by 20%
		and then we apply another effect with 30%,
		second one, should override the first one.

		Problems:
		1. Effects, are split into essentialy five parts:
		OnApply
		OnPeriod
		OnRemoved
		OnExpired
		EffectModifier.

		First four modify attributes directly on character.
		EffectModifier, modify attributes on other effects within OnApply(...)OnExpired.

		So if effect A have higher bonus to the same attribute than effect B,
		should we remove effect B entirely, or should we just remove it's EffectModifers.

		Modifiers, are still part of effect, but they modify other effects, are stored differently.
		More over what If one effect apply multiple modifiers, and only one of them is higher ?
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
	TSharedPtr<FGAActiveDuration> effect = ActiveEffects.FindRef(foundHandle);
	bool bIsStronger = false;
	if (effect.IsValid())
	{
		TArray<FGAAttributeData> data;
		data.Append(EffectIn.GetPeriodModifiers());
		data.Append(EffectIn.GetOnRemovedModifiers());
		data.Append(EffectIn.GetOnEndedModifiers());
		for (FGAAttributeData& am : data)
		{
			if (effect->ComparePeriodModifiers(am))
			{
				bIsStronger = true;
				break;
			}
		}
	}
	
	//for (FGEffectModifierGroup& group : EffectIn.EffectModifierGroups)
	//{
	//	FGAEffectModifiersCont* efModHan = EffectMods.Modifiers.Find(group.AttributeTag);
	//	if (efModHan)
	//	{
	//		for (FGAEffectModifierHandles& modHan : efModHan->Modifiers)
	//		{
	//			for (auto MoIt = modHan.Modifiers.CreateIterator(); MoIt; ++MoIt)
	//			{
	//				for (FGAEffectModifier& grMod : group.Modifiers)
	//				{
	//					if (MoIt->ModDirection == grMod.ModDirection
	//						&& MoIt->Mod == grMod.Mod
	//						&& grMod.DirectModifier.Value > MoIt->DirectModifier.Value)
	//					{
	//						modHan.Modifiers.RemoveAtSwap(MoIt.GetIndex());
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	RemoveActiveEffect(foundHandle);
	FGAEffectHandle handle = AddActiveEffect(EffectIn, Ctx);

	FGAEffectTagHandle nameHandle(EffectIn.EffectName, handle);
	instCont.Effects.Add(nameHandle);

	return handle;
}
FGAEffectHandle FGAActiveEffectContainer::HandleInstigatorEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAInstigatorEffectContainer& instCont = InstigatorEffects.FindOrAdd(Ctx.InstigatorComp);
	
	FGAEffectHandle foundHandle;

	RemoveActiveEffect(foundHandle);
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
			durationEffect->RestartTimer(EffectIn.EffectDuration.Duration);
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

	return FGAEffectHandle();
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