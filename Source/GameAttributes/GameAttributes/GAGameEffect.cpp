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

FGAGameEffect::FGAGameEffect(class UGAGameEffectSpec* GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffect(GameEffectIn),
	Calculation(GameEffect->CalculationType.GetDefaultObject()),
	Context(ContextIn)
{
	OwnedTags = GameEffectIn->OwnedTags;

	for(FGAGameEffectInfo& info : GameEffectIn->OnAppliedInfo)
	{
		FGAGameEffectMod Mod(info.Attribute, info.Value, info.ChangeType);
		OnAppliedMods.Add(Mod);
	}
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
			FGAEffectMod mod(info.Attribute, info.Value, info.ChangeType);
			ModsOut.Add(mod);
		}
	}
	return ModsOut;
}

FGAGameEffectHandle FGAGameEffectHandle::GenerateHandle(FGAGameEffect* EffectIn)
{
	static int32 GlobalEffectHandleID = 0;
	FGAGameEffectHandle NewHandle(GlobalEffectHandleID++, EffectIn);

	return NewHandle;
}

void FGAGameEffectContainer::ApplyEffect(const FGAGameEffect& EffectIn)
{
	//instant effect goes trough simplest application path
	//just make effect and run it trough modifiers.
	FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
	//FGAGameEffect Effect = EfNoConst.Calculation->ModiifyEffect(EffectIn);
	if(Effect.GameEffect->EffectType == EGAEffectType::Periodic)
	{
		//add to map of active effects ?
		//Or simply create timer here, execute by handle and simply add handle to array ?
	}
	ExecuteEffect(Effect);
}
void FGAGameEffectContainer::ExecuteEffect(FGAGameEffect& EffectIn)
{
	FGAGameEffect& Effect = const_cast<FGAGameEffect&>(EffectIn);
	FGACalculationContext InstiContext(EffectIn.Context.InstigatorComp.Get(), EffectIn.Context.InstigatorComp->DefaultAttributes, EGAModifierDirection::Outgoing);
	FGACalculationContext TargetContext(EffectIn.Context.TargetComp.Get(), EffectIn.Context.TargetComp->DefaultAttributes, EGAModifierDirection::Incoming);
	
	TArray<FGAEffectMod> OnAppliedMods = EffectIn.GetOnAppliedMods();
	for(FGAEffectMod& mod : OnAppliedMods)
	{
		EffectIn.Calculation->ModifyEffect(mod, InstiContext);
		EffectIn.Calculation->ModifyEffect(mod, TargetContext);
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