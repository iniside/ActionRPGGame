// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"

#include "GAEffectCue.h"

#include "GAAttributeComponent.h"
#include "GAEffectExecution.h"
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
		FGAGameEffectHandle& Handle = const_cast<FGAGameEffectHandle&>(HandleIn);
		FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
		Handle.GetContext().TargetComp->ExecuteEffect(Handle);
		break;
	}
	case EGAEffectType::Periodic:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Apply periodic effect"));
		FGAGameEffectHandle& Handle = const_cast<FGAGameEffectHandle&>(HandleIn);
		FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
		FTimerDelegate del = FTimerDelegate::CreateUObject(Handle.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExecuteEffect, Handle);
		FTimerManager& timer = Handle.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timer.SetTimer(Handle.GetEffectRef().PeriodTimerHandle, del,
			EffectIn.GameEffect->Period, true, 0);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Handle.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, Handle);
		FTimerManager& timerDuration = Handle.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timerDuration.SetTimer(Handle.GetEffectRef().DurationTimerHandle, delDuration,
			EffectIn.GameEffect->Duration, false);

		break;
	}
	case EGAEffectType::Duration:
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("UGAAttributeComponent:: Apply effect for duration"));
		FGAGameEffectHandle& Handle = const_cast<FGAGameEffectHandle&>(HandleIn);
		FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);

		Effect.Context.TargetComp->ApplyEffectForDuration(Handle);

		FTimerDelegate delDuration = FTimerDelegate::CreateUObject(Handle.GetContext().TargetComp.Get(), &UGAAttributeComponent::ExpireEffect, Handle);
		FTimerManager& timerDuration = Handle.GetEffectRef().Context.TargetComp->GetWorld()->GetTimerManager();

		timerDuration.SetTimer(Handle.GetEffectRef().DurationTimerHandle, delDuration,
			EffectIn.GameEffect->Duration, false);

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

void FGAGameEffectContainer::RemoveEffect(FGAGameEffectHandle& HandleIn)
{
	TSharedPtr<FGAGameEffect> effect = ActiveEffects.FindRef(HandleIn);
	if (effect.IsValid())
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer:: Removing Effect"))
		effect.Reset();
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