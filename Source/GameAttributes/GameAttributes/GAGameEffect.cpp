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

FGAAttributeBase* FGAExecutionContext::GetTargetAttribute(const FGAAttribute& AttributeIn)
{
	return TargetAttributes->GetAttribute(AttributeIn);
}
FGAAttributeBase* FGAExecutionContext::GetInstigatorAttribute(const FGAAttribute& AttributeIn)
{
	return InstigatorAttributes->GetAttribute(AttributeIn);
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
TArray<FGAEffectMod> FGAGameEffect::GetOnPeriodMods()
{
	return GetMods(GameEffect->OnPeriodInfo);
}
TArray<FGAEffectMod> FGAGameEffect::GetOnDurationMods()
{
	return GetMods(GameEffect->OnDurationInfo);
}
void FGAGameEffect::OnPeriod()
{
	GetTargetComp()->ExecuteEffect(*this, EGAModifierApplication::OnPeriod);
};
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
				FGAEffectMod mod(info.Attribute, info.DirectModifier.GetValue() , info.ChangeType, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::AttributeBased:
			{
				FGAEffectMod mod(info.Attribute, info.AttributeBased.GetValue(Context), info.ChangeType, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::CurveBased:
			{
				FGAEffectMod mod(info.Attribute, info.CurveBased.GetValue(Context), info.ChangeType, GameEffect);
				ModsOut.Add(mod);
				break;
			}
			case EGAMagnitudeCalculation::CustomCalculation:
			{
				FGAEffectMod mod(info.Attribute, info.Custom.GetValue(Context), info.ChangeType, GameEffect);
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

void FGAGameEffect::InitializePeriodic()
{
	//apply OnApplied modifiers once, if there are any.
	GetTargetComp()->ExecuteEffect(*this, EGAModifierApplication::OnApplied);

	FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAGameEffect::OnPeriod);
	FTimerManager& timer = GetTargetComp()->GetWorld()->GetTimerManager();

	timer.SetTimer(PeriodTimerHandle, del, 1, true);
}
void FGAGameEffect::ExecuteEffect(FGAGameEffect* Effect, FGAEffectMod& ModIn, FGAExecutionContext& ExecContextIn)
{
	Execution->ExecuteEffect(Effect, ModIn, ExecContextIn);
}
void FGAGameEffectContainer::ApplyEffect(const FGAGameEffect& EffectIn
	, const FGAGameEffectHandle& HandleIn)
{
	//instant effect goes trough simplest application path
	//just make effect and run it trough modifiers.
	
	//FGAGameEffect Effect = EfNoConst.Calculation->ModiifyEffect(EffectIn);
	switch(EffectIn.GameEffect->EffectType)
	{
	case EGAEffectType::Instant:
	{
		//dont do anything fancy now simply execute effect and forget about it.
		FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
		Effect.GetTargetComp()->ExecuteEffect(Effect, EGAModifierApplication::OnApplied);
		break;
	}
	case EGAEffectType::Periodic:
	{
		FGAGameEffectHandle& Handle = const_cast<FGAGameEffectHandle&>(HandleIn);
		ActiveEffects.Add(HandleIn, Handle.GetEffectPtr());
		Handle.GetEffectRef().InitializePeriodic();
		break;
	}
	default:
		break;
	}
	//regardless of what happen try to apply modifiers
	for (FGAGameEffectModifier& mod : EffectIn.GameEffect->Modifiers)
	{
		ApplyModifier(mod, EffectIn);
	}
	
}
void FGAGameEffectContainer::ExecuteEffect(FGAGameEffect& EffectIn,
	EGAModifierApplication ModAppType)
{
	FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
	FGACalculationContext InstiContext(EffectIn.Context.InstigatorComp.Get(), EffectIn.Context.InstigatorComp->DefaultAttributes, EGAModifierDirection::Outgoing);
	FGACalculationContext TargetContext(EffectIn.Context.TargetComp.Get(), EffectIn.Context.TargetComp->DefaultAttributes, EGAModifierDirection::Incoming);
	
	FGAExecutionContext ExecContext(EffectIn.Context.TargetComp.Get(), EffectIn.Context.TargetComp->DefaultAttributes,
		EffectIn.Context.InstigatorComp.Get(), EffectIn.Context.InstigatorComp->DefaultAttributes);
	
	switch(ModAppType)
	{
		case EGAModifierApplication::OnApplied:
		{
			TArray<FGAEffectMod> OnAppliedMods = EffectIn.GetOnAppliedMods();
			UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Number Of Mods to apply = %f"), OnAppliedMods.Num());
			for (FGAEffectMod& mod : OnAppliedMods)
			{
				Effect.ExecuteEffect(&Effect, mod, ExecContext);
			}
			break;
		}
		case EGAModifierApplication::OnPeriod:
		{
			//UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffect::OnPeriod not implemented"));
			TArray<FGAEffectMod> OnPeriodMods = EffectIn.GetOnPeriodMods();
			UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Number Of Mods to apply = %f"), OnPeriodMods.Num());
			for (FGAEffectMod& mod : OnPeriodMods)
			{
				Effect.ExecuteEffect(&Effect, mod, ExecContext);
			}
			break;
		}
		case EGAModifierApplication::OnDuration:
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffect::OnDuration not implemented"));
			break;
		}
		case EGAModifierApplication::OnExpired:
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffect::OnExpired not implemented"));
			break;
		}
		case EGAModifierApplication::OnRemoved:
		{
			UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffect::OnRemoved not implemented"));
			break;
		}
	}
	
	//OwningComp->DefaultAttributes->ModifyAttribute(Effect);
}

void FGAGameEffectContainer::ExecutePeriodicEffect(FGAGameEffectHandle HandleIn)
{
	//FGAGameEffect Effect = *HandleIn.EffectPtr.Get();
	//
	//FGACalculationContext InstiContext(Effect.Context.InstigatorComp.Get(), Effect.Context.InstigatorComp->DefaultAttributes, EGAModifierDirection::Outgoing);
	//FGACalculationContext TargetContext(Effect.Context.TargetComp.Get(), Effect.Context.TargetComp->DefaultAttributes, EGAModifierDirection::Incoming);
	//for (FGAGameEffectMod& mod : Effect.OnAppliedMods)
	//{
	//	Effect.Calculation->ModifyEffectInstigator(Effect, mod, InstiContext);
	//	Effect.Calculation->ModifyEffectTarget(Effect, mod, TargetContext);
	//}
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
			FGAAttributeBase* baseAttribute =  attr->GetAttribute(ModifierIn.Attribute);
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