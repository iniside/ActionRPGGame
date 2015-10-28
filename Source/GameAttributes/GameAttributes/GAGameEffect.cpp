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
#include "GAAttributeComponent.h"
#include "GAGameEffect.h"

FGAGameEffectHandle FGAGameEffectHandle::GenerateHandle(FGAGameEffect* EffectIn)
{
	static int32 Handle;
	Handle++;
	return FGAGameEffectHandle(Handle, EffectIn);
}

FGAGameEffect::FGAGameEffect(class UGAGameEffectSpec* GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffect(GameEffectIn),
	Calculation(GameEffect->CalculationType.GetDefaultObject()),
	Context(ContextIn)
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

TArray<FGAEffectMod> FGAGameEffect::GetMods(TArray<FGAGameEffectInfo>& ModsInfoIn)
{
	TArray<FGAEffectMod> ModsOut;
	if (GameEffect)
	{
		for (FGAGameEffectInfo& info : ModsInfoIn)
		{
			FGAEffectMod mod(info.Attribute, info.Value, info.ChangeType, GameEffect);
			ModsOut.Add(mod);
		}
	}
	return ModsOut;
}

void FGAGameEffect::Initialize()
{
	FTimerDelegate del = FTimerDelegate::CreateRaw(this, &FGAGameEffect::OnPeriod);
	FTimerManager& timer = GetTargetComp()->GetWorld()->GetTimerManager();
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
		Effect.GetTargetComp()->ExecuteEffect(Effect);
		break;
	}
	case EGAEffectType::Periodic:
		break;
	default:
		return;
	}
	
}
void FGAGameEffectContainer::ExecuteEffect(FGAGameEffect& EffectIn)
{
	FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
	FGACalculationContext InstiContext(EffectIn.Context.InstigatorComp.Get(), EffectIn.Context.InstigatorComp->DefaultAttributes, EGAModifierDirection::Outgoing);
	FGACalculationContext TargetContext(EffectIn.Context.TargetComp.Get(), EffectIn.Context.TargetComp->DefaultAttributes, EGAModifierDirection::Incoming);
	
	TArray<FGAEffectMod> OnAppliedMods = EffectIn.GetOnAppliedMods();
	UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Number Of Mods to apply = %f"), OnAppliedMods.Num());
	for(FGAEffectMod& mod : OnAppliedMods)
	{
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Premodified Mod Value = %f"), mod.Value);
		EffectIn.Calculation->ModifyEffect(EffectIn, mod, InstiContext);
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Post Instigator modified Value = %f"), mod.Value);
		EffectIn.Calculation->ModifyEffect(EffectIn, mod, TargetContext);
		UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::ExecuteEffect: Post Target modified Value = %f"), mod.Value);
		OwningComp->DefaultAttributes->ModifyAttribute(mod);
	}
	OwningComp->DefaultAttributes->ModifyAttribute(Effect);
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

void FGAGameEffectContainer::ApplyModifier(const FGAGameEffectModifier& ModifierIn)
{
	int32 Index = static_cast<int32>(ModifierIn.Direction);
	Modifiers[Index].Add(ModifierIn);
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