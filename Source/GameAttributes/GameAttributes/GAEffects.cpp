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

FGAEffectInstant::FGAEffectInstant(const FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn)
{
	Modifiers = SpecIn.EvalModifiers;
}


void FGAActiveDuration::ActivateEffect()
{
	FTimerDelegate durationDel = FTimerDelegate::CreateRaw(this, &FGAActiveDuration::OnEnded);;
	Context.Target->GetWorldTimerManager().SetTimer(DurationTimerHandle, durationDel, 10, false);
}
void FGAActiveDuration::FinishEffect()
{
	if (Context.Target.IsValid())
		Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
}
FGAActiveDuration::FGAActiveDuration(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
	const FGAEffectHandle& HandleIn)
{
	MyHandle = HandleIn;
	Context = ContextIn;
	OnAppliedModifiers = SpecIn.GetModifiers();
	OnEndedModifiers = SpecIn.GetOnEndedModifiers();
	OnRemovedModifiers = SpecIn.GetOnRemovedModifiers();
}
FGAActiveDuration::~FGAActiveDuration()
{
	FinishEffect();
	Context.Reset();
}

void FGAActivePeriodic::OnPeriod()
{
	if (Context.TargetComp.IsValid() && Context.InstigatorComp.IsValid())
		Context.InstigatorComp->ModifyAttributesOnTarget(PeriodModifiers, Context, MyHandle);
}
void FGAActivePeriodic::OnApplied()
{
	if (Context.TargetComp.IsValid() && Context.InstigatorComp.IsValid())
		Context.InstigatorComp->ModifyAttributesOnTarget(OnAppliedModifiers, Context, MyHandle);
}
void FGAActivePeriodic::OnEnded()
{
	Context.TargetComp->EffectExpired(MyHandle);
}
void FGAActivePeriodic::ActivateEffect()
{
	FTimerDelegate periodDel = FTimerDelegate::CreateRaw(this, &FGAActivePeriodic::OnPeriod);
	Context.Target->GetWorldTimerManager().SetTimer(PeriodTimerHandle, periodDel, 1, true, 0);

	FTimerDelegate durationDel = FTimerDelegate::CreateRaw(this, &FGAActivePeriodic::OnEnded);;
	Context.Target->GetWorldTimerManager().SetTimer(DurationTimerHandle, durationDel, 10, false);
}
void FGAActivePeriodic::FinishEffect()
{
	if (Context.Target.IsValid())
	{
		Context.Target->GetWorldTimerManager().ClearTimer(PeriodTimerHandle);
		Context.Target->GetWorldTimerManager().ClearTimer(DurationTimerHandle);
	}
}

FGAActivePeriodic::FGAActivePeriodic(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
	const FGAEffectHandle& HandleIn)
	//	: Context(ContextIn)
{
	MyHandle = HandleIn;
	Context = ContextIn;
	PeriodModifiers = SpecIn.GetPeriodModifiers(); //we probabaly want to recalculate it on every tick
	//if effect.
	OnAppliedModifiers = SpecIn.GetModifiers();
	OnEndedModifiers = SpecIn.GetOnEndedModifiers();
	OnRemovedModifiers = SpecIn.GetOnRemovedModifiers();
}

FGAActivePeriodic::~FGAActivePeriodic()
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

void FGATaggedModifiers::CalculateNewModifierStack()
{
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
void FGATaggedModifiers::AddModifierEffect(const FGAEffectHandle& HandleIn, const FGAEvalData& Eval)
{
	TArray<FGAEvalData>& EvalData = EffectMods.FindOrAdd(HandleIn);
	EvalData.Add(Eval);
	CalculateNewModifierStack();
}
void FGAActiveEffectsModifiers::AddTaggedModifier(const FGameplayTagContainer& RequiredTargetTags,
	const FGameplayTagContainer& RequiredInstigatorTags,
	const FGAEffectHandle& HandleIn, const FGAEvalData& Eval)
{
	for (FGATaggedModifiers& tagMod : TaggedModifiers)
	{
		if (tagMod.RequiredTargetTags.MatchesAll(RequiredTargetTags, true)
			&& tagMod.RequiredInstigatorTags.MatchesAll(RequiredInstigatorTags, true))
		{
			tagMod.AddModifierEffect(HandleIn, Eval);
			return;
		}
	}
	FGATaggedModifiers tagMod;
	tagMod.RequiredTargetTags = RequiredTargetTags;
	tagMod.RequiredInstigatorTags = RequiredTargetTags;
	tagMod.AddModifierEffect(HandleIn, Eval);
	TaggedModifiers.Add(tagMod);
}
void FGATaggedModifiers::RemoveModifierEffect(const FGAEffectHandle& HandleIn)
{
	EffectMods.Remove(HandleIn);
	CalculateNewModifierStack();
}
int32 FGATaggedModifiers::GetModifierCount()
{
	return EffectMods.Num();
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

	//check tagged modifiers, if we can apply them.
	for (FGATaggedModifiers& tag : TaggedModifiers)
	{
		if (Ctx.InstigatorComp->AppliedTags.HasAllTags(tag.RequiredInstigatorTags, true)
			&& Ctx.TargetComp->AppliedTags.HasAllTags(tag.RequiredTargetTags, true))
		{
			switch (EvalData.ModDirection)
			{
			case EGAModifierDirection::Incoming:
			{
				EvalData.Value = EvalData.Value + (((EvalData.Value * tag.IncomingMultiplyMod)
					+ tag.IncomingAddtiveMod - tag.IncomingSubtractMod) / tag.IncomingDivideMod);
				break;
			}
			case EGAModifierDirection::Outgoing:
			{
				EvalData.Value = EvalData.Value + (((EvalData.Value * tag.OutgoingMultiplyMod)
					+ tag.OutgoingAddtiveMod - tag.OutgoingSubtractMod) / tag.OutgoingDivideMod);
				break;
			}
			default:
				break;
			}
		}
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
		//from that point on, we don't really want const.
		FGAEffectSpec& nonConst = const_cast<FGAEffectSpec&>(SpecIn);
		return HandlePeriodicEffect(nonConst, Ctx);
	}
	case EGAEffectType::Duration:
	{
		FGAEffectSpec& nonConst = const_cast<FGAEffectSpec&>(SpecIn);
		return HandleDurationEffect(nonConst, Ctx);
	}
	case EGAEffectType::Infinite:
	{
		return HandleInfiniteEffect(SpecIn, Ctx);
	}
	default:
	{
		return FGAEffectHandle();
	}
	}
}

void FGAActiveEffectContainer::ApplyEffectModifiers(FGAAttributeData& DataIn, const FGAEffectContext& Ctx)
{
	//if(Ctx.TargetComp->MyTags.MatchesAll(DataIn.TargetTagsRequiared, true)) 
	//{
	//ok, target have required tags
	//figure out which attribute this is modifing.
	//}
}
/*
Assuming we have effect +30% dmage to zombies, this implies:
1. It increase damage only if target, is of type zombie (like PawnType.Zombie tag).
2. It's modified as an outgoing effect (because WE instigator have it).
3. Before modifications will applied, we need to get owned tags from target.
4. We need to check if target have all required tags, for this modification.
5. If not we discard this effect.

Problems:
How do I check if this effect can modify outgoing effect ?
Simplest solution is to simply, brute force iterate over all active effects, while checking for their tags,
and discard all unfit effects.
*/
void FGAActiveEffectContainer::ExecuteEffectModifier(FGAAttributeData& ModifierIn, const FGAEffectContext& Ctx)
{
	/*
	Concept:
	1. Assuming Modifier in is coming with Damage.Fire tag (specialized);
	2. We have Damage.Ice bonus and Damage.Fire Bonus,
	3. Then only Damage.Fire bonus will be applied.
	Else
	2. We have Damage.Ice, Damage.Fire, and Damage bonus.
	3. In this case Damage.Fire, and Damage bonuses will be applied,
	since Damage.Fire can be split into two tags (Damage and Damage.Fire).
	Bonuses are applied in order they exist in in map.
	It would probabal best to sort it, from most generic to most specialized.
	*/

	//first we need to find attributes
	FGameplayTagContainer tags = ModifierIn.AttributeTags.GetGameplayTagParents();
	auto TagIt = tags.CreateConstIterator();
	for (TagIt; TagIt; ++TagIt)
	{
		//and we modify them.
		FGAActiveEffectsModifiers* mods = Modifiers.Find(*TagIt);
		if (mods)
			mods->ApplyModifiers(ModifierIn, Ctx);
	}

	//secons tep, would be to find attributes with associated requriment tags.
	//we get already calculated modifier, and modify by those more, specialized
	//modifier effects.

	//FGameplayTagContainer tags = ModifierIn.AttributeTags.GetGameplayTagParents();
	//auto TagIt = tags.CreateConstIterator();
	//for (TagIt; TagIt; ++TagIt)
	//{
	//	TArray<FGAActiveEffectsModifiers>* mods = AttributeModifiers.Find(*TagIt);
	//	if (mods)
	//	{
	//		//undeterministic behaviour
	//		//very narrow bonuses like +30 against creature type, might be applied, before or after
	//		//very problematic in case of percentages!
	//		for (FGAActiveEffectsModifiers& mod : *mods)
	//		{
	//			if (Ctx.TargetComp->AppliedTags.HasAllTags(mod.RequiredTargetTags, true)
	//				&& Ctx.InstigatorComp->AppliedTags.HasAllTags(mod.RequiredInstigatorTags, true))
	//			{
	//				mod.ApplyModifiers(ModifierIn);
	//			}
	//		}
	//	}
	//}

	//if (Modifiers.Num() > 0)
	//{
	//FGAActiveEffectsModifiers* mods = Modifiers.Find(ModifierIn.AttributeTag);
	//if (mods)
	//	mods->ApplyModifiers(ModifierIn);
	//}
}
void FGAActiveEffectContainer::RemoveActiveEffect(const FGAEffectHandle& HandleIn)
{
	TSharedPtr<FGAActiveBase> removedEffect;
	ActiveEffects.RemoveAndCopyValue(HandleIn, removedEffect);
	if (removedEffect.IsValid())
	{
		//uhhhuuhu, we need to clean up all handles from attributes as well
		for (FGAAttributeData& data : removedEffect->OnAppliedModifiers)
		{
			FGAAttributeBase* attr = removedEffect->Context.TargetComp->GetAttribute(data.Attribute);
			if (attr)
			{
				attr->RemoveBonus(HandleIn);
			}
		}
		for (FGameplayTag& attrTag : removedEffect->AttributeEffectModifiers)
		{
			FGAActiveEffectsModifiers* mod = Modifiers.Find(attrTag);
			mod->RemoveModifierEffect(HandleIn);
			//dat is terrible!
			for (FGATaggedModifiers& tag : mod->TaggedModifiers)
			{
				for (const FGEffectModifierGroup& modG : removedEffect->EffectModifiers)
				{
					for (const FGAEffectModifier& effMod : modG.Modifiers)
					{
						if (tag.RequiredInstigatorTags.MatchesAll(effMod.InstigatorRequiredTags, true)
							&& tag.RequiredTargetTags.MatchesAll(effMod.TargetRequiredTags, true))
						{
							tag.RemoveModifierEffect(HandleIn);
							if (tag.GetModifierCount() <= 0)
							{

							}
						}
					}
				}
			}
			if (mod->GetModifierCount() <= 0)
			{
				AttributeModifiers.Remove(attrTag);
			}
		}
		removedEffect->FinishEffect();
	}
}
FGAEffectHandle FGAActiveEffectContainer::HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx)
{
	if (Ctx.TargetComp.IsValid() && Ctx.InstigatorComp.IsValid())
	{
		FGAEffectHandle handle;
		Ctx.InstigatorComp->ModifyAttributesOnTarget(SpecIn.Modifiers, Ctx, handle);
	}
	return FGAEffectHandle();
}
FGAEffectHandle FGAActiveEffectContainer::HandlePeriodicEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	//TODO::
	//1. Comment this.
	//2. Handle stacking.
	//3. Handle conditional effect applying (if have this tags, apply this effect)
	//4. effect apply effect. Not really important for Instant effect, but Duration based, have 
	// stages at which additonal effects can be applied, and only those effects can know when to do it.
	//5. Add support for UObject effect extensions which are instanced per application, for special logic.

	FGAEffectHandle handle = FGAEffectHandle::GenerateHandle();
	TSharedPtr<FGAActivePeriodic> tempPeriodic = MakeShareable(new FGAActivePeriodic(Ctx, EffectIn, handle));
	tempPeriodic->ActivateEffect();

	for (const FGEffectModifierGroup& mod : EffectIn.EffectModifiers)
	{
		FGAActiveEffectsModifiers& aem = Modifiers.FindOrAdd(mod.AttributeTag);
		for (const FGAEffectModifier& effMod : mod.Modifiers)
		{
			FGAEvalData data(mod.Attribute, effMod.Mod, effMod.ModDirection, mod.AttributeTag, effMod.DirectModifier.Value);
			if (effMod.TargetRequiredTags.Num() > 0
				|| effMod.InstigatorRequiredTags.Num() > 0)
			{
				aem.AddTaggedModifier(effMod.TargetRequiredTags, effMod.InstigatorRequiredTags,
					handle, data);
			}
			else
				aem.AddModifierEffect(handle, data);
		}
		tempPeriodic->AttributeEffectModifiers.Add(mod.AttributeTag);
	}

	ActiveEffects.Add(handle, tempPeriodic);

	return handle;
}
FGAEffectHandle FGAActiveEffectContainer::HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	return FGAEffectHandle();
}
FGAEffectHandle FGAActiveEffectContainer::HandleInfiniteEffect(const FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx)
{
	FGAEffectHandle handle = FGAEffectHandle::GenerateHandle();
	//repro, press M, and Y,Y 
	TSharedPtr<FGAActiveInfinite> tempInfinite = MakeShareable(new FGAActiveInfinite());
	tempInfinite->MyHandle = handle;
	tempInfinite->Context = Ctx;
	tempInfinite->EffectModifiers = EffectIn.EffectModifiers;
	for (const FGEffectModifierGroup& mod : EffectIn.EffectModifiers)
	{

		FGAActiveEffectsModifiers& aem = Modifiers.FindOrAdd(mod.AttributeTag);
		for (const FGAEffectModifier& effMod : mod.Modifiers)
		{
			FGAEvalData data(mod.Attribute, effMod.Mod, effMod.ModDirection, mod.AttributeTag, effMod.DirectModifier.Value);

			if (effMod.TargetRequiredTags.Num() > 0
				|| effMod.InstigatorRequiredTags.Num() > 0)
			{
				aem.AddTaggedModifier(effMod.TargetRequiredTags, effMod.InstigatorRequiredTags,
					handle, data);
			}
			else
				aem.AddModifierEffect(handle, data);
		}
		tempInfinite->AttributeEffectModifiers.Add(mod.AttributeTag);

		//TArray<FGAActiveEffectsModifiers>& mods = AttributeModifiers.FindOrAdd(mod.AttributeTag);
		//TArray<FGAActiveEffectsModifiers> tempAEM;
		//for (const FGAEffectModifier& effMod : mod.Modifiers)
		//{
		//	FGAEvalData data(mod.Attribute, effMod.Mod, effMod.ModDirection, mod.AttributeTag, effMod.DirectModifier.Value);
		//	FGAActiveEffectsModifiers actMod;
		//	actMod.RequiredTargetTags.AppendTags(effMod.TargetRequiredTags);
		//	actMod.RequiredInstigatorTags.AppendTags(effMod.InstigatorRequiredTags);
		//	actMod.AddModifierEffect(handle, data);
		//	tempAEM.Add(actMod);
		//}
		//AttributeModifiers.Add(mod.AttributeTag, tempAEM);
		//tempInfinite->AttributeEffectModifiers.Add(mod.AttributeTag);
	}

	InfiniteEffects.Add(handle, tempInfinite);

	return handle;
}

void FGAActiveEffectContainer::RemoveInfiniteEffect(const FGAEffectHandle& HandleIn)
{
	TSharedPtr<FGAActiveInfinite> removedEffect; // InfiniteEffects.FindAndRemoveChecked(HandleIn);
	InfiniteEffects.RemoveAndCopyValue(HandleIn, removedEffect);
	if (removedEffect.IsValid())
	{
		for (FGameplayTag& attrTag : removedEffect->AttributeEffectModifiers)
		{
			FGAActiveEffectsModifiers* mod = Modifiers.Find(attrTag);
			mod->RemoveModifierEffect(HandleIn);
			//dat is terrible!
			for (FGATaggedModifiers& tag : mod->TaggedModifiers)
			{
				for (const FGEffectModifierGroup& modG : removedEffect->EffectModifiers)
				{
					for (const FGAEffectModifier& effMod : modG.Modifiers)
					{
						if (tag.RequiredInstigatorTags.MatchesAll(effMod.InstigatorRequiredTags, true)
							&& tag.RequiredTargetTags.MatchesAll(effMod.TargetRequiredTags, true))
						{
							tag.RemoveModifierEffect(HandleIn);
							if (tag.GetModifierCount() <= 0)
							{

							}
						}
					}
				}
			}
			if (mod->GetModifierCount() <= 0)
			{
				AttributeModifiers.Remove(attrTag);
			}
		}
	}
}

void FGAActiveEffectContainer::Clean()
{
	Modifiers.Empty();

	auto efIt = ActiveEffects.CreateIterator();
	//for (efIt; efIt; ++efIt)
	//	{
	//		efIt->Value.Get()->FinishEffect();
	//	}
	ActiveEffects.Empty();
}