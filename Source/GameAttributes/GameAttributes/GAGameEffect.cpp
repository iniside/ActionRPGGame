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
#include "GAGameEffect.h"


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
}

void FGAGameEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
}

TArray<FGAEffectMod> FGAGameEffect::GetOnAppliedMods()
{
	return GetMods(GameEffect->OnAppliedInfo);
}
void FGAGameEffect::OnDuration()
{

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

void FGAGameEffect::DurationExpired()
{

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
		/*FTimerDelegate del = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExecuteEffect, HandleIn);
		FTimerManager& timer = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timer.SetTimer(HandleIn.GetEffectRef().PeriodTimerHandle, del,
			EffectIn.GameEffect->Period, true, 0);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
		FTimerManager& timerDuration = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timerDuration.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
			EffectIn.GameEffect->Duration, false);*/

		InternalCheckEffectStacking(HandleIn);
		InternalApplyModifiers(HandleIn);
		break;
	}
	case EGAEffectType::Duration:
	{

		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Apply effect for duration"));
		HandleIn.GetContextRef().TargetComp->ApplyEffectForDuration(HandleIn);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(HandleIn.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, HandleIn);
		FTimerManager& timerDuration = HandleIn.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timerDuration.SetTimer(HandleIn.GetEffectRef().DurationTimerHandle, delDuration,
			EffectIn.GameEffect->Duration, false);

		InternalCheckEffectStacking(HandleIn);
		InternalApplyModifiers(HandleIn);
		break;
	}
	default:
		break;
	}
	//apply additonal effect applied with this effect.
	for (TSubclassOf<UGAGameEffectSpec> Spec : EffectIn.GameEffect->OnAppliedEffects)
	{
		FGAGameEffectHandle Handle = EffectIn.Context.TargetComp->MakeGameEffect(Spec, EffectIn.Context);

		EffectIn.Context.TargetComp->ApplyEffectToSelf(Handle.GetEffect(), Handle);
	}
	//regardless of what happen try to apply effect modifiers
	for (FGAGameEffectModifier& mod : EffectIn.GameEffect->Modifiers)
	{
		ApplyModifier(mod, EffectIn);
	}

}
void FGAGameEffectContainer::InternalCheckEffectStacking(const FGAGameEffectHandle& HandleIn)
{
	UGAGameEffectSpec* Spec = HandleIn.GetEffectSpec();
	//sEGAEffectStacking Stacking = Spec->EffectStacking;
	EGAEffectAggregation Aggregation = Spec->EffectAggregation;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	UE_LOG(GameAttributes, Log, TEXT("Stacking Type: %s"), *EnumToString::GetStatckingAsString(Stacking));
	switch (Aggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
	{
		TMap<UClass*, FGAGameEffectHandle>* EffectHandle = InstigatorEffectHandles.Find(HandleIn.GetContextRef().Instigator.Get());
		FGAGameEffectHandle* Handle = nullptr;
		if(EffectHandle)
			Handle = EffectHandle->Find(HandleIn.GetEffectSpec()->StaticClass());
		
		switch (Stacking)
		{
		case EGAEffectStacking::Add:
		{
			InternalApplyEffect(HandleIn);
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
			InternalApplyEffect(HandleIn);
			break;
		}
		case EGAEffectStacking::StrongerOverride:
		{
			if (Handle)
			{
				RemoveEffect(*Handle);
			}
			InternalApplyEffect(HandleIn);
			break;
		}
		}
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
	{
		break;
	}
	}
	InternalApplyEffectByAggregation(HandleIn);
}
void FGAGameEffectContainer::InternalApplyModifiers(const FGAGameEffectHandle& HandleIn)
{
	TArray<FGAGameEffectModifier>& Modifiers = HandleIn.GetEffectSpec()->Modifiers;
	EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
	for (FGAGameEffectModifier& Modifier : Modifiers)
	{
		if (Modifier.Attribute.IsValid())
		{
			UGAAttributesBase* attr = HandleIn.GetContextRef().GetTargetAttributes();
			FGAAttributeBase* Attribute = attr->GetAttribute(Modifier.Attribute);

			if (Attribute)
			{
				Attribute->AddBonus(FGAModifier(Modifier.ModType, Modifier.Value), HandleIn, Stacking);
			}
		}
	}
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
		TMap<UClass*, FGAGameEffectHandle>& EffectByClass = InstigatorEffectHandles.FindOrAdd(HandleIn.GetContextRef().Instigator.Get());
		EffectByClass.Add(HandleIn.GetEffectSpec()->StaticClass(), HandleIn);
		break;
	}
	case EGAEffectAggregation::AggregateByTarget:
		TargetEffects.Add(HandleIn, HandleIn.GetEffectPtr());
		break;
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
	EGAEffectAggregation aggregatiopn = HandleIn.GetEffectRef().GameEffect->EffectAggregation;
	UObject* Instigator = HandleIn.GetContextRef().Instigator.Get();
	TSharedPtr<FGAGameEffect> effect = ActiveEffects.FindAndRemoveChecked(HandleIn);

	if (effect.IsValid())
	{
		switch (aggregatiopn)
		{
		case EGAEffectAggregation::AggregateByInstigator:
		{
			TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>>* effects = InstigatorEffects.Find(Instigator);
			TMap<UClass*, FGAGameEffectHandle>* EffectByClass = InstigatorEffectHandles.Find(Instigator);
			if (EffectByClass)
			{
				EffectByClass->Remove(HandleIn.GetEffectSpec()->StaticClass());
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
			TargetEffects.FindAndRemoveChecked(HandleIn);
			break;
		}
		}
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
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer:: Removing Effect"))
			effect.Reset();
	}
}

void FGAGameEffectContainer::ApplyEffectInstance(class UGAEffectInstanced* EffectIn)
{
	//do something ?
	switch (EffectIn->EffectAggregation)
	{
	case EGAEffectAggregation::AggregateByInstigator:
		InstigatorInstancedEffects.Add(EffectIn->Context.Instigator.Get(), EffectIn);
		break;
	case EGAEffectAggregation::AggregateByTarget:
		break;
	}
}

FGAGameEffectContainer::FGAGameEffectContainer()
{
	Modifiers.SetNum(2);
}

void FGAGameEffectContainer::ApplyModifier(const FGAGameEffectModifier& ModifierIn,
	const FGAGameEffect& EffectIn)
{
	//if there is valid attribute specified, we go different path and ignore any tags modifiers.
	if (ModifierIn.Attribute.IsValid())
	{
		FGAGameEffect& effect = const_cast<FGAGameEffect&>(EffectIn);
		UGAAttributesBase* attr = effect.Context.GetTargetAttributes();
		//it shouldn't ever be null
		if (attr)
		{
			FGAAttributeBase* baseAttribute = attr->GetAttribute(ModifierIn.Attribute);
		}
	}
	else
	{
		int32 Index = static_cast<int32>(ModifierIn.Direction);
		Modifiers[Index].Add(ModifierIn);
	}
}

FGAGameModifierStack FGAGameEffectContainer::GetQualifableMods(const FGAGameEffect& EffectIn
	, const FGACalculationContext& Context)
{
	FGAGameModifierStack ModifierStackOut;
	int32 Index = static_cast<int32>(Context.Direction);
	for (const FGAGameEffectModifier& mod : Modifiers[Index])
	{
		if (mod.RequiredTags.MatchesAny(EffectIn.OwnedTags, false) && mod.Direction == Context.Direction)
		{
			switch (mod.ModType)
			{
			case EGAAttributeMod::Add:
				UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : %d"), *EffectIn.OwnedTags.ToString(), mod.Value, *EnumToString::GetModifierDirectionAsString(Context.Direction));
				ModifierStackOut.Additive += mod.Value;
				UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Mod Value: %f"), ModifierStackOut.Additive);
				break;
			case EGAAttributeMod::Subtract:
				UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : %d"), *EffectIn.OwnedTags.ToString(), mod.Value, *EnumToString::GetModifierDirectionAsString(Context.Direction));
				ModifierStackOut.Subtractive += mod.Value;
				UE_LOG(GameAttributesEffects, Log, TEXT("Total Subtractive Mod Value: %f"), ModifierStackOut.Subtractive);
				break;
			case EGAAttributeMod::Multiply:
				UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : %d"), *EffectIn.OwnedTags.ToString(), mod.Value, *EnumToString::GetModifierDirectionAsString(Context.Direction));
				ModifierStackOut.Multiply += mod.Value;
				UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Multiply Value: %f"), ModifierStackOut.Multiply);
				break;
			case EGAAttributeMod::Divide:
				UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : %d"), *EffectIn.OwnedTags.ToString(), mod.Value, *EnumToString::GetModifierDirectionAsString(Context.Direction));
				ModifierStackOut.Divide += mod.Value;
				UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Divide Value: %f"), ModifierStackOut.Divide);
				break;
			default:
				break;
			}
		}
	}
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