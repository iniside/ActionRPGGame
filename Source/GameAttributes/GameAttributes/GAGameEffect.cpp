// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"

#include "GAEffectCue.h"

#include "GAAttributeComponent.h"
#include "GAEffectExecution.h"
#include "GAEffectInstanced.h"
#include "GAEffectInstanced.h"
#include "GAGameEffect.h"

DEFINE_STAT(STAT_GatherModifiers);

FGAGameEffectHandle FGAGameEffectHandle::GenerateHandle(FGAGameEffect* EffectIn)
{
	static int32 Handle;
	Handle++;
	return FGAGameEffectHandle(Handle, EffectIn);
}

void FGAGameEffectHandle::ExecuteEffect(FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAExecutionContext& Context)
{
	GetEffectRef().Execution->ExecuteEffect(HandleIn, ModIn, Context);
}

FGAGameEffect::FGAGameEffect(class UGAGameEffectSpec* GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffect(GameEffectIn),
	Context(ContextIn),
	Execution(GameEffect->ExecutionType.GetDefaultObject())
{
	OwnedTags = GameEffectIn->OwnedTags;
	TargetWorld = ContextIn.Target->GetWorld();
	AppliedTime = TargetWorld->TimeSeconds;
	LastTickTime = TargetWorld->TimeSeconds;
}

void FGAGameEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}

TArray<FGAEffectMod> FGAGameEffect::GetAttributeModifiers()
{
	return GetMods(GameEffect->AtributeModifiers);
}
void FGAGameEffect::OnDuration()
{

}
void FGAGameEffect::OnExpired()
{
	OnEffectExpired.ExecuteIfBound();
}
void FGAGameEffect::OnRemoved()
{
	OnEffectRemoved.ExecuteIfBound();
}
void FGAGameEffect::DurationExpired()
{

}
float FGAGameEffect::GetActivationTime()
{
	return GameEffect->Duration;
}
float FGAGameEffect::GetCurrentActivationTime()
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - AppliedTime;
}
float FGAGameEffect::GetCurrentTickTime()
{
	float CurrentTime = TargetWorld->TimeSeconds;
	return CurrentTime - LastTickTime;
}

TArray<FGAEffectMod> FGAGameEffect::GetMods(TArray<FGAAttributeModifier>& ModsInfoIn)
{
	TArray<FGAEffectMod> ModsOut;
	if (GameEffect)
	{
		for (FGAAttributeModifier& info : ModsInfoIn)
		{
			switch (info.CalculationType)
			{
			case EGAMagnitudeCalculation::Direct:
			{
				FGAEffectMod mod(info.Attribute, info.DirectModifier.GetValue(), info.AttributeMod, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::AttributeBased:
			{
				FGAEffectMod mod(info.Attribute, info.AttributeBased.GetValue(Context), info.AttributeMod, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::CurveBased:
			{
				FGAEffectMod mod(info.Attribute, info.CurveBased.GetValue(Context), info.AttributeMod, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::CustomCalculation:
			{
				FGAEffectMod mod(info.Attribute, info.Custom.GetValue(Context), info.AttributeMod, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			default:
				break;
			}
		}
	}
	return ModsOut;
}

float FGAEffectModifiersContainer::GatherMods(const FGameplayTagContainer& TagsIn, const TMap<FGAGameEffectHandle, TArray<FGAGameEffectModifier>>& Data)
{
	//possible optimization when needed - create separate thread.

	float ModifierVal = 0;
	float Add = 0;
	float Multiply = 1;
	float Subtract = 0;
	float Divide = 1;
	float PercentageAdd = 0;
	float PercentageSubtract = 0;
	for (auto It = Data.CreateConstIterator(); It; ++It)
	{
		for (const FGAGameEffectModifier& Test : It->Value)
		{
			if (TagsIn.MatchesAll(Test.RequiredTags, false))
			{
				switch (Test.ModType)
				{
				case EGAAttributeMod::Add:
					Add += Test.Value;
					break;
				case EGAAttributeMod::Multiply:
					Multiply += Test.Value;
					break;
				case EGAAttributeMod::Subtract:
					Subtract += Test.Value;
					break;
				case EGAAttributeMod::Divide:
					Divide += Test.Value;
					break;
				case EGAAttributeMod::PercentageAdd:
					PercentageAdd += Test.Value;
					break;
				case EGAAttributeMod::PercentageSubtract:
					PercentageSubtract += Test.Value;
					break;
				default:
					break;
				}
			}
		}
	}
	ModifierVal = ((Add - Subtract) * Multiply) / Divide;
	ModifierVal = ModifierVal + (ModifierVal * PercentageAdd);
	ModifierVal = ModifierVal - (ModifierVal * PercentageSubtract);
	SCOPE_CYCLE_COUNTER(STAT_GatherModifiers);
	return ModifierVal;
}

float FGAEffectModifiersContainer::GetIncomingMods(const FGameplayTagContainer& TagsIn)
{
	return GatherMods(TagsIn, IncomingEffectModifiers);
}
/* Outgoing mods are for effects which I'm applying to other target. */
float FGAEffectModifiersContainer::GetOutgoingMods(const FGameplayTagContainer& TagsIn)
{
	return GatherMods(TagsIn, OutgoingEffectModifiers);
}
void FGAEffectModifiersContainer::AddEffectModifier(const FGAGameEffectHandle& HandleIn, const FGAGameEffectModifier& Modifier, EGAModifierDirection Direction)
{
	switch (Direction)
	{
	case EGAModifierDirection::Incoming:
	{
		TArray<FGAGameEffectModifier>& Mods = IncomingEffectModifiers.FindOrAdd(HandleIn);
		Mods.Add(Modifier);
		break;
	}
	case EGAModifierDirection::Outgoing:
	{
		TArray<FGAGameEffectModifier>& Mods = OutgoingEffectModifiers.FindOrAdd(HandleIn);
		Mods.Add(Modifier);
		break;
	}
	default:
		return;
	}
}
void FGAEffectModifiersContainer::RemoveModifiers(const FGAGameEffectHandle& HandleIn)
{
	IncomingEffectModifiers.Remove(HandleIn);
	OutgoingEffectModifiers.Remove(HandleIn);
}
void FGAGameEffectContainer::ApplyEffect(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	//instant effect goes trough simplest application path
	//just make effect and run it trough modifiers.

	//FGAGameEffect Effect = EfNoConst.Calculation->ModiifyEffect(EffectIn);
	switch (EffectIn.GameEffect->EffectType)
	{
	case EGAEffectType::Instant:
	{
		//dont do anything fancy now simply execute effect and forget about it.
		HandleIn.GetContext().TargetComp->ExecuteEffect(HandleIn);
		break;
	}
	case EGAEffectType::Periodic:
	{

		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Apply periodic effect"));
		InternalCheckPeriodicEffectStacking(HandleIn);
		break;
	}
	case EGAEffectType::Duration:
	{

		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Apply effect for duration"));
		InternalCheckDurationEffectStacking(HandleIn);
		break;
	}
	case EGAEffectType::Infinite:
	{
		break;
	}
	default:
		break;
	}
	//apply additonal effect applied with this effect.
	for (TSubclassOf<UGAGameEffectSpec> Spec : EffectIn.GameEffect->OnAppliedEffects)
	{
		FGAGameEffectHandle Handle = EffectIn.Context.TargetComp->MakeGameEffect(Spec, EffectIn.Context);

		EffectIn.Context.InstigatorComp->ApplyEffectToTarget(Handle.GetEffect(), Handle);
	}
	//regardless of what happen try to apply effect modifiers
	for (FGAGameEffectModifier& mod : EffectIn.GameEffect->Modifiers)
	{
		ApplyEffectModifier(mod, HandleIn);
	}

}
void FGAGameEffectContainer::InternalCheckDurationEffectStacking(const FGAGameEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	UE_LOG(GameAttributes, Log, TEXT("Stacking Type: %s"), *EnumToString::GetStatckingAsString(Stacking));
	FGAGameEffectHandle* Handle = nullptr;
	switch (Aggregation)
	{
		case EGAEffectAggregation::AggregateByInstigator:
		{
			TMap<FName, TSet<FGAGameEffectHandle>>* EffectHandle = InstigatorEffectHandles.Find(HandleIn.GetContextRef().Instigator.Get());
			TSet<FGAGameEffectHandle>* HandleSet = nullptr;
			if (EffectHandle)
			{
				HandleSet = EffectHandle->Find(HandleIn.GetEffectSpec()->GetFName());
				if (HandleSet)
				{
					Handle = HandleSet->Find(HandleIn);
				}
			}
		}
		case EGAEffectAggregation::AggregateByTarget:
		{
			TSet<FGAGameEffectHandle>* HandleSet = TargetEffectByType.Find(HandleIn.GetEffectSpec()->GetFName());
			if (HandleSet)
			{
				Handle = HandleSet->Find(HandleIn);
			}
			break;
		}
	}
	switch (Stacking)
	{
		case EGAEffectStacking::Add:
		{
			InternalApplyDuration(HandleIn);
			break;
		}
		case EGAEffectStacking::Duration:
		{
			if (Handle)
			{
				InternalExtendEffectDuration(*Handle, HandleIn);
			}
			else
			{
				FGAGameEffectHandle InvalidHandle;
				InternalExtendEffectDuration(HandleIn, InvalidHandle);
			}
			break;
		}
		case EGAEffectStacking::Intensity:
		{
			break;
		}
		case EGAEffectStacking::Override:
		{
			if (Handle)
			{
				RemoveEffect(*Handle);
			}
			InternalApplyDuration(HandleIn);
			break;
		}
		case EGAEffectStacking::StrongerOverride:
		{
			if (Handle)
			{
				RemoveEffect(*Handle);
			}
			InternalApplyDuration(HandleIn);
			break;
		}
	}

	InternalApplyEffectByAggregation(HandleIn);
}

void FGAGameEffectContainer::InternalCheckPeriodicEffectStacking(const FGAGameEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	UE_LOG(GameAttributes, Log, TEXT("Stacking Type: %s"), *EnumToString::GetStatckingAsString(Stacking));
	FGAGameEffectHandle* Handle = nullptr;
	switch (Aggregation)
	{
		case EGAEffectAggregation::AggregateByInstigator:
		{
			TMap<FName, TSet<FGAGameEffectHandle>>* EffectHandle = InstigatorEffectHandles.Find(HandleIn.GetContextRef().Instigator.Get());
			TSet<FGAGameEffectHandle>* HandleSet = nullptr;
			if (EffectHandle)
			{
				HandleSet = EffectHandle->Find(HandleIn.GetEffectSpec()->GetFName());
				if (HandleSet)
				{
					Handle = HandleSet->Find(HandleIn);
				}
			}
		}
		case EGAEffectAggregation::AggregateByTarget:
		{
			TSet<FGAGameEffectHandle>* HandleSet = TargetEffectByType.Find(HandleIn.GetEffectSpec()->GetFName());
			if (HandleSet)
			{
				Handle = HandleSet->Find(HandleIn);
			}
			break;
		}
	}
	switch (Stacking)
	{
		case EGAEffectStacking::Add:
		{
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
		case EGAEffectStacking::Override:
		{
			break;
		}
		case EGAEffectStacking::StrongerOverride:
		{
			InternalApplyPeriodic(HandleIn);
			break;
		}
	}
	InternalApplyEffectByAggregation(HandleIn);
}
void FGAGameEffectContainer::InternalApplyPeriodic(const FGAGameEffectHandle& HandleIn)
{
	FTimerDelegate delPeriod = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExecuteEffect, HandleIn);
	FTimerManager& PeriodicTimer = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();
	float period = HandleIn.GetEffectSpec()->Period;
	PeriodicTimer.SetTimer(HandleIn.GetEffectRef().PeriodTimerHandle, delPeriod,
		period, true, 0);

	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();
	float Duration = HandleIn.GetEffectSpec()->Duration;
	timerDuration.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
		Duration, false, Duration);

	InternalApplyEffectTags(HandleIn);
}
void FGAGameEffectContainer::InternalApplyDuration(const FGAGameEffectHandle& HandleIn)
{
	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

	timerDuration.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
		HandleIn.GetEffectSpec()->Duration, false, HandleIn.GetEffectSpec()->Duration);

	TArray<FGAEffectMod> Modifiers = HandleIn.GetEffectRef().GetAttributeModifiers();
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	InternalApplyEffectTags(HandleIn);
	for (FGAEffectMod& Modifier : Modifiers)
	{
		if (Modifier.Attribute.IsValid())
		{
			UGAAttributesBase* attr = HandleIn.GetContextRef().GetTargetAttributes();
			FGAAttributeBase* Attribute = attr->GetAttribute(Modifier.Attribute);

			if (Attribute)
			{
				Attribute->AddBonus(FGAModifier(Modifier.AttributeMod, Modifier.Value), HandleIn, Stacking);
			}
		}
	}
}
void FGAGameEffectContainer::InternalApplyEffectTags(const FGAGameEffectHandle& HandleIn)
{
	//wayt to few dereferences!
	UGAAttributeComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	FGAGameEffect& Effect = HandleIn.GetEffectRef();

	Target->AppliedTags.AddTagContainer(Effect.ApplyTags);
}
void FGAGameEffectContainer::InternalApplyEffectByAggregation(const FGAGameEffectHandle& HandleIn)
{
	EGAEffectAggregation Aggregation = HandleIn.GetEffectRef().GameEffect->EffectAggregation;

	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>>& EffectMap = InstigatorEffects.FindOrAdd(HandleIn.GetContextRef().Instigator.Get());
		EffectMap.Add(HandleIn, HandleIn.GetEffectPtr());
		TMap<FName, TSet<FGAGameEffectHandle>>& EffectByClass = InstigatorEffectHandles.FindOrAdd(HandleIn.GetContextRef().Instigator.Get());
		TSet<FGAGameEffectHandle>& EffectSet = EffectByClass.FindOrAdd(HandleIn.GetEffectSpec()->GetFName());
		EffectSet.Add(HandleIn);

		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		TSet<FGAGameEffectHandle>& Handles = TargetEffectByType.FindOrAdd(HandleIn.GetEffectSpec()->GetFName());
		Handles.Add(HandleIn);
		//TargetEffects.Add(HandleIn, HandleIn.GetEffectPtr());

		break;
	}
	}
	ActiveEffects.Add(HandleIn, HandleIn.GetEffectPtr());
}
void FGAGameEffectContainer::InternalApplyEffect(const FGAGameEffectHandle& HandleIn)
{
	UE_LOG(GameAttributesEffects, Log, TEXT("InternalApplyEffect"));
	if (HandleIn.GetEffectSpec()->EffectType == EGAEffectType::Periodic)
	{
		FTimerDelegate delPeriod = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExecuteEffect, HandleIn);
		FTimerManager& PeriodicTimer = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		PeriodicTimer.SetTimer(HandleIn.GetEffectRef().PeriodTimerHandle, delPeriod,
			HandleIn.GetEffectSpec()->Period, true, 0);
	}
	FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
	FTimerManager& timerDuration = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

	timerDuration.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
		HandleIn.GetEffectSpec()->Duration, false);
}
void FGAGameEffectContainer::InternalExtendEffectDuration(const FGAGameEffectHandle& HandleIn, const FGAGameEffectHandle& ExtendingHandleIn)
{
	if (ExtendingHandleIn.IsValid())
	{
		FTimerManager& DurationTimer = HandleIn.GetContextRef().TargetComp->GetWorld()->GetTimerManager();
		float RemainingTime = DurationTimer.GetTimerRemaining(HandleIn.GetEffectRef().DurationTimerHandle);
		float NewDuration = RemainingTime + ExtendingHandleIn.GetEffectSpec()->Duration;
		DurationTimer.ClearTimer(HandleIn.GetEffectRef().DurationTimerHandle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
		DurationTimer.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
			NewDuration, false);
	}
	else
	{
		InternalApplyEffect(HandleIn);
	}
}
void FGAGameEffectContainer::RemoveEffect(FGAGameEffectHandle& HandleIn)
{
	if (!ActiveEffects.Contains(HandleIn))
	{
		UE_LOG(GameAttributes, Log, TEXT("RemoveEffect Effect %s Is not applied"), *HandleIn.GetEffectRef().ToString());
		return;
	}
	EGAEffectAggregation aggregatiopn = HandleIn.GetEffectRef().GameEffect->EffectAggregation;
	UObject* Instigator = HandleIn.GetContextRef().Instigator.Get();
	UGAAttributeComponent* Target = HandleIn.GetContextRef().TargetComp.Get();
	TSharedPtr<FGAGameEffect> effect = ActiveEffects.FindAndRemoveChecked(HandleIn);

	if (effect.IsValid())
	{
		switch (aggregatiopn)
		{
			case EGAEffectAggregation::AggregateByInstigator:
			{
				TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>>* effects = InstigatorEffects.Find(Instigator);
				TMap<FName, TSet<FGAGameEffectHandle>>* EffectByClass = InstigatorEffectHandles.Find(Instigator);
				if (EffectByClass)
				{
					//Probabaly need another path for removing just single effect from stack.
					EffectByClass->Remove(HandleIn.GetEffectSpec()->GetFName());
				}
				if (effects)
				{
					effects->FindAndRemoveChecked(HandleIn);
					if (effects->Num() == 0)
					{
						InstigatorEffects.Remove(Instigator);
					}
				}
				break;
			}
			case EGAEffectAggregation::AggregateByTarget:
			{
				//TargetEffects.FindAndRemoveChecked(HandleIn);
				TSet<FGAGameEffectHandle>* Handles = TargetEffectByType.Find(HandleIn.GetEffectSpec()->GetFName());
				//check aggregation type to know which effect to remove exactly ?
				TargetEffectByType.Remove(HandleIn.GetEffectSpec()->GetFName());
				break;
			}
		}
		Target->AppliedTags.RemoveTagContainer(effect->ApplyTags);
		for (FGAGameEffectModifier& Modifier : effect->GameEffect->Modifiers)
		{
			if (Modifier.Attribute.IsValid())
			{
				FGAAttributeBase* Attribute = OwningComp->GetAttribute(Modifier.Attribute);
				if (Attribute)
				{
					Attribute->RemoveBonus(HandleIn);
				}
			}
		}
		effect->OnEffectRemoved.ExecuteIfBound();
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer:: Removing Effect"))
		//effect.Reset();
	}
}

FGAGameEffectContainer::FGAGameEffectContainer()
{
}

void FGAGameEffectContainer::ApplyEffectInstance(class UGAEffectInstanced* EffectIn)
{
	//do something ?
	switch (EffectIn->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		FGAInstigatorInstancedEffectContainer& Effects = InstigatorInstancedEffects.FindOrAdd(EffectIn->Context.Instigator.Get());
		Effects.Effects.AddUnique(EffectIn);
		//VERY BAD CHANGE IT.
		EffectIn->Initialize(EffectIn->Context);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		break;
	}
	}
}
void FGAGameEffectContainer::RemoveEffectInstance(class UGAEffectInstanced* EffectIn)
{
	switch (EffectIn->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		FGAInstigatorInstancedEffectContainer* Effects = InstigatorInstancedEffects.Find(EffectIn->Context.Instigator.Get());
		if (Effects)
		{
			Effects->Effects.Remove(EffectIn);
			if (Effects->Effects.Num() == 0)
			{
				InstigatorEffects.Remove(EffectIn->Context.Instigator.Get());
			}
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		TargetInstancedEffects.Remove(EffectIn);
		break;
	}
	}
}

void FGAGameEffectContainer::ApplyEffectModifier(const FGAGameEffectModifier& ModifierIn,
	const FGAGameEffectHandle& HandleIn)
{
	EffectModifiers.AddEffectModifier(HandleIn, ModifierIn, ModifierIn.Direction);
}

bool FGAGameEffectContainer::IsEffectActive(const FGAGameEffectHandle& HandleIn)
{
	if (ActiveEffects.Contains(HandleIn))
	{
		return true;
	}
	return false;
}

FGAGameModifierStack FGAGameEffectContainer::GetQualifableMods(const FGAGameEffect& EffectIn
	, const FGACalculationContext& Context)
{
	FGAGameModifierStack ModifierStackOut;

	return ModifierStackOut;
}
FGAGameModifierStack FGACalculationContext::GetModifiers(const FGAGameEffect& EffectIn
	, const FGACalculationContext& Context)
{
	return AttributeComp->GameEffectContainer.GetQualifableMods(EffectIn, Context);
}

UGAGameEffectSpec::UGAGameEffectSpec()
{

}