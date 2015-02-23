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
	for (FGAAttributeModifier& mod : AttributeModifiers)
	{
		EvalModifiers.Add(mod.GetModifier(Context));
	}
	return EvalModifiers;
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
	Duration = SpecIn.EffectDuration.Duration;
	Period = SpecIn.EffectDuration.Period;
	MyHandle = HandleIn;
	Context = ContextIn;
	OwnedTags = SpecIn.EffectTags;
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

void FGAActiveEffectsModifiers::ApplyModifiers(FGAAttributeData& EvalData, const FGAEffectContext& Ctx)
{
	switch (EvalData.ModDirection)
	{
	case EGAModifierDirection::Incoming:
	{
		EvalData.Value = EvalData.Value + (((EvalData.Value * IncomingMultiplyMod)
			+ IncomingAddtiveMod - IncomingSubtractMod) / IncomingDivideMod);
		break;
	}
	case EGAModifierDirection::Outgoing:
	{
		EvalData.Value = EvalData.Value + (((EvalData.Value * OutgoingMultiplyMod)
			+ OutgoingAddtiveMod - OutgoingSubtractMod) / OutgoingDivideMod);
		break;
	}
	default:
		break;
	}
	//EvalData.Value = ((EvalData.Value * MultiplyMod) + AddtiveMod - SubtractMod) / DivideMod;
}

void FGAActiveEffectsModifiers::AddModifierEffect(const FGAEffectHandle& HandleIn, const FGAEvalData& Eval)
{
	TArray<FGAEvalData>& EvalData = EffectMods.FindOrAdd(HandleIn);
	EvalData.Add(Eval);

	//EffectMods.Add(HandleIn, Eval);
	CalculateNewModifierStack();
}

void FGAActiveEffectsModifiers::RemoveModifierEffect(const FGAEffectHandle& HandleIn)
{
	EffectMods.Remove(HandleIn);
	CalculateNewModifierStack();
}
int32 FGAActiveEffectsModifiers::GetModifierCount()
{
	return EffectMods.Num();
}
void FGAActiveEffectsModifiers::CalculateNewModifierStack()
{
	//reset modifiers we have right now.
	OutgoingAddtiveMod = 0;
	OutgoingSubtractMod = 0;
	OutgoingMultiplyMod = 0;
	OutgoingDivideMod = 1;

	IncomingAddtiveMod = 0;
	IncomingSubtractMod = 0;
	IncomingMultiplyMod = 0;
	IncomingDivideMod = 1;

	auto ModIt = EffectMods.CreateIterator();
	//we will just accumulate mods on stack.
	//we don't do anything special with them.
	for (ModIt; ModIt; ++ModIt)
	{
		for (FGAEvalData& eval : ModIt->Value)
		{
			switch (eval.ModDirection)
			{
			case EGAModifierDirection::Incoming:
			{
				switch (eval.Mod)
				{
				case EGAAttributeMod::Add:
					IncomingAddtiveMod += eval.Value;
					break;
				case EGAAttributeMod::Subtract:
					IncomingSubtractMod += eval.Value;
					break;
				case EGAAttributeMod::Multiply:
					IncomingMultiplyMod += eval.Value;
					break;
				case EGAAttributeMod::Divide:
					IncomingDivideMod += eval.Value;
					break;
				case EGAAttributeMod::Set:
					break;
				case EGAAttributeMod::Invalid:
					break;
				}
			}
			case EGAModifierDirection::Outgoing:
			{
				switch (eval.Mod)
				{
				case EGAAttributeMod::Add:
					OutgoingAddtiveMod += eval.Value;
					break;
				case EGAAttributeMod::Subtract:
					OutgoingSubtractMod += eval.Value;
					break;
				case EGAAttributeMod::Multiply:
					OutgoingMultiplyMod += eval.Value;
					break;
				case EGAAttributeMod::Divide:
					OutgoingDivideMod += eval.Value;
					break;
				case EGAAttributeMod::Set:
					break;
				case EGAAttributeMod::Invalid:
					break;
				}
			}
			default:
				break;
			}

		}
	}
}
void FGAEffectModifierHandles::AddMod(const FGAEffectModifier& ModIn)
{
	Modifiers.Add(ModIn);
}

void FGAEffectModifiersContainer::AddMods(const FGAEffectHandle& HandleIn, 
	const TArray<FGAEffectModifier>& ModsIn)
{
	TArray<FGAEffectModifier>& found = AllModifiers.FindOrAdd(HandleIn);
	found.Append(ModsIn);
	//if (ModsIn.Num() > 0)
	//{
	//	for (const FGEffectModifierGroup& group : ModsIn)
	//	{
	//		//TArray<FGAEffectModifier>& found = AllModifiers.FindOrAdd(HandleIn);
	//		//found.Append(group.Modifiers);
	//		FGAEffectModifiersCont& attributeHandles = Modifiers.FindOrAdd(group.AttributeTag);
	//		FGAEffectModifierHandles mods(HandleIn, group.Modifiers);
	//		attributeHandles.Modifiers.Add(mods);
	//	}
	//}
}
void FGAEffectModifiersContainer::RemoveMods(const FGAEffectHandle& HandleIn)
{
	AllModifiers.Remove(HandleIn);
	//for (auto ModIt = Modifiers.CreateIterator(); ModIt; ++ModIt)
	//{
	//	for (auto HanIt = ModIt->Value.Modifiers.CreateIterator(); HanIt; ++HanIt)
	//	{
	//		if (HanIt->Handle == HandleIn)
	//		{
	//			ModIt->Value.Modifiers.RemoveAtSwap(HanIt.GetIndex());
	//		}
	//	}
	//}
}
void FGAEffectModifiersContainer::RemoveMod(const FGAEffectHandle& HandleIn, FGameplayTagContainer& ModTags)
{
	TArray<FGAEffectModifier>* found = AllModifiers.Find(HandleIn);
	if (found)
	{
		for (auto ModIt = found->CreateIterator(); ModIt; ++ModIt)
		{
			if (ModIt->RequiredTags.MatchesAll(ModTags, false))
			{
				found->RemoveAtSwap(ModIt.GetIndex());
			}
				
		}
	}
}
void FGAEffectModifiersContainer::StackModifiers(FGAEffectSpec& SpecIn)
{
	//for (FGAEffectModifier& mod : SpecIn.EffectModifiers)
	//{
	//	FGAActiveEffectModifier& activeMod = Modifiers.FindOrAdd(mod.AttributeTag);

	//	//this really should be global rule
	//	//not per effect ?
	//	switch (mod.ModStacking)
	//	{
	//		case EGAModifierEffectStacking::CantStackSameType:
	//		{
	//			SpecIn.EffectTags.MatchesAny(activeMod.AffectingEffectsTags, false);
	//			break;
	//		}
	//		case EGAModifierEffectStacking::SameTypeOverride:
	//		{
	//			SpecIn.EffectTags.MatchesAny(activeMod.AffectingEffectsTags, false);
	//			break;
	//		}
	//		case EGAModifierEffectStacking::StackAll:
	//		{
	//			break;
	//		}
	//	}
	//	
	//	//find this effect, and compare it to new one.

	//}
}
void FGAEffectModifiersContainer::ApplyModifiersToEffect(FGAAttributeData& EffectIn, const FGameplayTagContainer& EffectTags
	, const FGAEffectContext& Ctx)
{
	FGameplayTagContainer tranAttr = EffectIn.AttributeTags.GetGameplayTagParents();
	TArray<FGAEffectModifier> QualifiedMods;
	FGACountedTagContainer& instigatorTags = Ctx.InstigatorComp->AppliedTags;
	FGACountedTagContainer& targetTags = Ctx.TargetComp->AppliedTags;
	//for (const FGameplayTag& attr : tranAttr)
	//{
	//	/*
	//		If highest override
	//		and Damage.Fire is higher than Damage, we need to only take damage.
	//		So we need to discard rest.
	//	*/
	//	FGAEffectModifiersCont& attributeHandles = Modifiers.FindOrAdd(attr);
	//	auto HanIt = attributeHandles.Modifiers.CreateConstIterator();
	//	for (HanIt; HanIt; ++HanIt)
	//	{
	//		auto AtrIt = HanIt->Modifiers.CreateConstIterator();
	//		for (AtrIt; AtrIt; ++AtrIt)
	//		{
	//			if (EffectTags.MatchesAll(AtrIt->RequiredTags, true)
	//				&& instigatorTags.HasAllTags(AtrIt->InstigatorRequiredTags, true)
	//				&& targetTags.HasAllTags(AtrIt->TargetRequiredTags, true))
	//			{
	//				QualifiedMods.Add(*AtrIt);
	//			}
	//		}
	//	}
	//}

	//auto ModIt = AllModifiers.CreateIterator();
	//for (ModIt; ModIt; ++ModIt)
	//{
	//	/*
	//		Say incoming effect have Hex, Damage.Fire tags.
	//		We have mod, which Require Damage.Fire,
	//		Hit! We have a match.

	//		Say we have mod which Require. Conjuration, Damage.Fire
	//		Miss! All tags, must much and we miss on Conjuration.
	//	*/
	//	/*
	//		This way we gather all qualifable mods, including spec mods like +20% damage vs undead
	//		while holding one Handed sword.

	//		Of course. We might want, to gather those mods in separate pass, and add them on top.. Or something.
	//	*/
	//	/*
	//		Do we need stacking rules checking here, or we can assume 
	//		that it was checked on effect application ?
	//		We need some checking here to determine what takes precedence Damage or Damage.Fire (for example)
	//	*/
	auto MoIt = AllModifiers.CreateIterator();
	for (MoIt; MoIt; ++MoIt)
	{
		for (FGAEffectModifier& mod : MoIt->Value)
		{
			if (mod.Attribute == EffectIn.Attribute) //move to map, with more specialize attributes ?
			{
				if (EffectTags.MatchesAll(mod.RequiredTags, true)
					&& instigatorTags.HasAllTags(mod.InstigatorRequiredTags, true)
					&& targetTags.HasAllTags(mod.TargetRequiredTags, true))
				{
					QualifiedMods.Add(mod);
				}
			}
		}
	}
	
	float OutgoingAddtiveMod = 0;
	float OutgoingSubtractMod = 0;
	float OutgoingMultiplyMod = 0;
	float OutgoingDivideMod = 1;

	float IncomingAddtiveMod = 0;
	float IncomingSubtractMod = 0;
	float IncomingMultiplyMod = 0;
	float IncomingDivideMod = 1;
	//calculate magnitude from QualifiedMods
	for (FGAEffectModifier& mod : QualifiedMods)
	{
		switch (mod.ModDirection)
		{
			case EGAModifierDirection::Outgoing:
			{
				switch (mod.Mod)
				{
				case EGAAttributeMod::Add:
					OutgoingAddtiveMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Subtract:
					OutgoingSubtractMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Multiply:
					OutgoingMultiplyMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Divide:
					OutgoingDivideMod += mod.DirectModifier.Value;
					break;
				}
			}
			case EGAModifierDirection::Incoming:
			{
				switch (mod.Mod)
				{
				case EGAAttributeMod::Add:
					IncomingAddtiveMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Subtract:
					IncomingSubtractMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Multiply:
					IncomingMultiplyMod += mod.DirectModifier.Value;
					break;
				case EGAAttributeMod::Divide:
					IncomingDivideMod += mod.DirectModifier.Value;
					break;
				}
			}
		}
	}

	switch (EffectIn.ModDirection)
	{
	case EGAModifierDirection::Incoming:
	{
		EffectIn.Value = (EffectIn.Value + IncomingAddtiveMod - IncomingSubtractMod);
		EffectIn.Value = (EffectIn.Value + (EffectIn.Value * IncomingMultiplyMod)) / IncomingDivideMod;
		break;
	}
	case EGAModifierDirection::Outgoing:
	{
		EffectIn.Value = (EffectIn.Value + OutgoingAddtiveMod - OutgoingSubtractMod);
		EffectIn.Value = (EffectIn.Value + (EffectIn.Value * OutgoingMultiplyMod)) / OutgoingDivideMod;
		break;
	}
	default:
		break;
	}
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
	EffectMods.ApplyModifiersToEffect(ModifierIn, EffectTags, Ctx);
}
void FGAActiveEffectContainer::RemoveActiveEffect(const FGAEffectHandle& HandleIn)
{
	TSharedPtr<FGAActiveDuration> removedEffect;
	ActiveEffects.RemoveAndCopyValue(HandleIn, removedEffect);
	if (removedEffect.IsValid())
	{
		//EffectMods.RemoveMods(removedEffect->MyHandle);
		////uhhhuuhu, we need to clean up all handles from attributes as well
		//for (FGAAttributeData& data : removedEffect->OnAppliedModifiers)
		//{
		//	FGAAttributeBase* attr = removedEffect->Context.TargetComp->GetAttribute(data.Attribute);
		//	if (attr)
		//	{
		//		attr->RemoveBonus(HandleIn);
		//	}
		//}
		
		removedEffect->FinishEffect();
	}
}

FGAEffectHandle FGAActiveEffectContainer::AddActiveEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle handle = FGAEffectHandle::GenerateHandle();
	TSharedPtr<FGAActiveDuration> tempPeriodic = MakeShareable(new FGAActiveDuration(Ctx, EffectIn, handle));
	tempPeriodic->ActivateEffect();

	EffectMods.AddMods(handle, EffectIn.EffectModifiers);
	
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
	for (FGAEffectTagHandle& eff : instCont.Effects)
	{
		if (eff.EffectName == EffectIn.EffectName)
		{
			foundHandle = eff.Handle;
			break;
		}
	}

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
	return foundHandle;
}
FGAEffectHandle FGAActiveEffectContainer::HandleTargetAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	switch (EffectIn.Policy.Stacking)
	{
	case EGAEffectStacking::StrongerOverride:
	{
		return HandleTargetEffectStrongerOverride(EffectIn, Ctx);
		break;
	}
	case EGAEffectStacking::Override:
	{
		return CheckTargetEffectOverride(EffectIn, Ctx);
		break;
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
		break;
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
FGAEffectHandle FGAActiveEffectContainer::CheckTargetEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle foundHandle;
	return foundHandle;
}


void FGAActiveEffectContainer::Clean()
{

	ActiveEffects.Empty();
}