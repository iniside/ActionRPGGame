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

#include "GAGameEffect.h"

FGAGameEffect::FGAGameEffect(class UGAGameEffectSpec* GameEffectIn,
	const FGAEffectContext& ContextIn)
	: GameEffect(GameEffectIn),
	Attribute(GameEffectIn->Attribute),
	Value(GameEffectIn->Value),
	ChangeType(GameEffectIn->ChangeType),
	Context(ContextIn)
{
	OwnedTags = GameEffectIn->OwnedTags;
}

void FGAGameEffect::SetContext(const FGAEffectContext& ContextIn)
{
	Context = ContextIn;
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
	FGAGameEffect& EfNoConst = const_cast<FGAGameEffect&>(EffectIn);
	FGAGameEffect Effect = EfNoConst.GetCalculation()->ModiifyEffect(EffectIn);
	ExecuteEffect(Effect);
}
void FGAGameEffectContainer::ExecuteEffect(const FGAGameEffect& EffectIn)
{
	OwningComp->ExecuteEffect(EffectIn);
}

void FGAGameEffectContainer::ApplyModifier(const FGAGameEffectModifier& ModifierIn)
{
	switch (ModifierIn.Direction)
	{
	case EGAModifierDirection::Incoming:
		IncomingModifiers.Add(ModifierIn);
		break;
	case EGAModifierDirection::Outgoing:
		OutgoingModifiers.Add(ModifierIn);
		break;
	default:
		break;
	}
}

FGAGameModifierStack FGAGameEffectContainer::GetQualifableMods(const FGAGameEffect& EffectIn, EGAModifierDirection Direction)
{
	FGAGameModifierStack ModifierStackOut;
	if (Direction == EGAModifierDirection::Outgoing)
	{
		for (const FGAGameEffectModifier& mod : OutgoingModifiers)
		{
			if (mod.RequiredTags.MatchesAny(EffectIn.OwnedTags, false))
			{
				switch (mod.ModType)
				{
				case EGAAttributeMod::Add:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Outgoing"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Additive += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Mod Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Subtract:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Outgoing"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Subtractive += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Subtractive Mod Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Multiply:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Outgoing"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Multiply += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Multiply Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Divide:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Outgoing"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Divide += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Divide Value: %f"), ModifierStackOut.Additive);
					break;
				default:
					break;
				}
			}
		}
	}
	else if(Direction == EGAModifierDirection::Incoming)
	{
		for (const FGAGameEffectModifier& mod : IncomingModifiers)
		{
			if (mod.RequiredTags.MatchesAny(EffectIn.OwnedTags, false))
			{
				switch (mod.ModType)
				{
				case EGAAttributeMod::Add:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Incoming"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Additive += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Mod Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Subtract:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Incoming"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Subtractive += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Subtractive Mod Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Multiply:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Incoming"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Multiply += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Multiply Value: %f"), ModifierStackOut.Additive);
					break;
				case EGAAttributeMod::Divide:
					UE_LOG(GameAttributesEffects, Log, TEXT("FGAGameEffectContainer::GetQualifableMods: %f , Value: %g : Incoming"), *EffectIn.OwnedTags.ToString(), mod.Value);
					ModifierStackOut.Divide += mod.Value;
					UE_LOG(GameAttributesEffects, Log, TEXT("Total Additive Divide Value: %f"), ModifierStackOut.Additive);
					break;
				default:
					break;
				}
			}
		}
		
	}
	return ModifierStackOut;
}

UGAGameEffectSpec::UGAGameEffectSpec()
{

}