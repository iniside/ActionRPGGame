// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "GameplayTagContainer.h"
#include "GAAbilitiesComponent.h"
#include "GAAttributesBase.h"
#include "IGAAbilities.h"

#include "GAAttributeBase.h"
DEFINE_STAT(STAT_CalculateBonus);
DEFINE_STAT(STAT_CurrentBonusByTag);
DEFINE_STAT(STAT_FinalBonusByTag);
//UGAAttributeBase::UGAAttributeBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//
//}

void FGAAttributeBase::InstantApplication(const FGAModifier& ModifierIn)
{
	switch (ModifierIn.AttributeMod)
	{
	case EGAAttributeMod::Add:
		CurrentValue += ModifierIn.Value;
		break;
	case EGAAttributeMod::Subtract:
		CurrentValue -= ModifierIn.Value;
		break;
	default:
		break;
	}
}
float FGAAttributeBase::Modify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn)
{
	if (HandleIn.GetEffectSpec()->EffectType == EGAEffectType::Duration
		|| HandleIn.GetEffectSpec()->EffectType == EGAEffectType::Infinite)
	{
		EGAEffectStacking Stacking = HandleIn.GetEffectSpec()->EffectStacking;
		AddBonus(FGAModifier(ModIn.AttributeMod, ModIn.Value, HandleIn), HandleIn, Stacking);
		return ModIn.Value;
	}
	else
	{
		switch (ModIn.AttributeMod)
		{
		case EGAAttributeMod::Add:
		{
			float OldCurrentValue = CurrentValue;
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: OldCurrentValue: %f"), OldCurrentValue);
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: AddValue: %f"), ModIn.Value);
			float Val = CurrentValue - (OldCurrentValue + ModIn.Value);
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: ActuallAddVal: %f"), Val);
			CurrentValue -= Val;
			CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: CurrentValue: %f"), CurrentValue);
			return CurrentValue;
		}
		case EGAAttributeMod::Subtract:
		{
			float OldCurrentValue = CurrentValue;
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: OldCurrentValue: %f"), OldCurrentValue);
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: SubtractValue: %f"), ModIn.Value);
			float Val = CurrentValue - (OldCurrentValue - ModIn.Value);
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: ActuallSubtractVal: %f"), Val);
			CurrentValue -= Val;
			CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
			UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: CurrentValue: %f"), CurrentValue);
			return CurrentValue;
		}
		case EGAAttributeMod::Multiply:
			return -1;
		case EGAAttributeMod::Divide:
			return -1;
		}
	}
	return -1;
}
bool FGAAttributeBase::CheckIfStronger(const FGAEffectHandle& HandleIn)
{
	bool bIsStronger = false;
	FGAEffectMod ModIn = HandleIn.GetAttributeModifier();
	FGAModifier mod(ModIn.AttributeMod, ModIn.Value, HandleIn);
	auto It = Modifiers.CreateConstIterator();
	for( It; It; ++It)
	{
		const FGAModifier& local = It->Value;
		bool bTagsMatch = local.Handle.HasAllTags(HandleIn.GetOwnedTags());// HandleIn.HasAllTags(local.Handle.GetOwnedTags());
		if (local.AttributeMod == ModIn.AttributeMod
			&& mod.Value > local.Value
			&& bTagsMatch)
		{
			bIsStronger = true;
			break;
		}
	}
	return bIsStronger;
}
void FGAAttributeBase::AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle
	, EGAEffectStacking StackingType)
{
	/*switch (StackingType)
	{
	case EGAEffectStacking::Override:
		RemoveBonusByType(ModifiersIn.AttributeMod);
		break;
	case EGAEffectStacking::StrongerOverride:
		RemoveWeakerBonus(ModifiersIn.AttributeMod, ModifiersIn.Value);
		break;
	}*/

	FGAModifier& modsTemp = Modifiers.FindOrAdd(Handle);
	modsTemp = ModifiersIn;
	CalculateBonus();
}
void FGAAttributeBase::RemoveBonus(const FGAEffectHandle& Handle)
{
	Modifiers.Remove(Handle);
	CalculateBonus();
}

void FGAAttributeBase::RemoveBonusByType(EGAAttributeMod ModType)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		const FGAModifier& mod = It->Value;
		//check if current attribute mod have the same mod
		if (mod.AttributeMod == ModType)
		{
			Modifiers.Remove(It.Key());
		}
	}
	CalculateBonus();
}
void FGAAttributeBase::RemoveWeakerBonus(EGAAttributeMod ModType, float ValueIn)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		const FGAModifier& mod = It->Value;
			//check if current attribute mod have the same mod
			//and is smaller than the the incoming one.
			if (mod.AttributeMod == ModType
				&& mod.Value <= ValueIn)
			{
				if (mod.AttributeMod == ModType)
				{
					Modifiers.Remove(It.Key());
				}
			}

	}
	CalculateBonus();
}

void FGAAttributeBase::RemoveBonusType(EGAAttributeMod ModType)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		const FGAModifier& mod = It->Value;
			//check if current attribute mod have the same mod
			if (mod.AttributeMod == ModType)
			{
				Modifiers.Remove(It.Key());
			}
	}
	CalculateBonus();
}

void FGAAttributeBase::CalculateBonus()
{
	SCOPE_CYCLE_COUNTER(STAT_CalculateBonus);
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 1;
	float DivideBonus = 1;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		const FGAModifier& mod = ModIt->Value;
			switch (mod.AttributeMod)
			{
			case EGAAttributeMod::Add:
				AdditiveBonus += mod.Value;
				break;
			case EGAAttributeMod::Subtract:
				SubtractBonus += mod.Value;
				break;
			case EGAAttributeMod::Multiply:
				MultiplyBonus += mod.Value;
				break;
			case EGAAttributeMod::Divide:
				DivideBonus += mod.Value;
				break;
			default:
				break;
			}
	}
	float OldBonus = BonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	BonusValue = (AdditiveBonus - SubtractBonus);
	BonusValue = (BonusValue * MultiplyBonus);
	BonusValue = (BonusValue / DivideBonus);
	//this is absolute maximum (not clamped right now).
	float addValue = BonusValue - OldBonus;
	//reset to max = 200
	CurrentValue = CurrentValue + addValue;
	/*
	BaseValue = 200;
	CurrentValue = 200;
	BonusValue = 50;
	CurrentValue = 200 + 50;
	CurentValue == 250;

	Damage taken.
	CurrentValue = 250 - 25;
	CurrentValue == 225;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 175; (225 - 50)
	OldBonusValue = 50;
	BonusValue = 0;
	CurrentValue == 225;
	BonusValue - OldBonusValue = -50;
	CurrentValue = CurrentValue + (-50); ??

	TwoBonuses 50 + 50;
	CurrentValue = 300 - 25;
	CurrentValue == 275;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 225; (275 - 50)
	OldBonusValue = 100;
	BonusValue = 50;
	CurrentValue == 225;
	BonusValue - OldBonusValue = -50;
	CurrentValue = CurrentValue + (-50); ??

	Inverse Bonus is going to be Increased:
	TwoBonuses 50 + 50;
	CurrentValue = 300 - 25;
	CurrentValue == 275;
	Bonus Ended - THERE IS NO SUBTRACTION ONLY FULL STATCK RECALCULATION;
	Expected Result : 325; (275 + 50)
	OldBonusValue = 100;
	new BonusValue = 150; (new bonus gives +50)
	CurrentValue == 275;
	BonusValue - OldBonusValue = 50; (150 - 100) = 50
	CurrentValue = CurrentValue + (50); ??
	*/
}
float FGAAttributeBase::GetCurrentValueByTags(const FGameplayTagContainer& TagsIn, FGAIndividualMods& Bonuses) const
{
	SCOPE_CYCLE_COUNTER(STAT_CurrentBonusByTag);
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 0;
	float DivideBonus = 1;
	float PercentageAddBonus = 0;
	float PercentageSubtractBonus = 0;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		const FGAModifier& mod = ModIt->Value;
			//if (mod.Handle->HasAllTags(TagsIn))
			if (ModIt->Key.HasAllTags(TagsIn))
			{
				switch (mod.AttributeMod)
				{
				case EGAAttributeMod::Add:
					AdditiveBonus += mod.Value;
					break;
				case EGAAttributeMod::Subtract:
					SubtractBonus += mod.Value;
					break;
				case EGAAttributeMod::Multiply:
					MultiplyBonus += mod.Value;
					break;
				case EGAAttributeMod::Divide:
					DivideBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageAdd:
					PercentageAddBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageSubtract:
					PercentageSubtractBonus += mod.Value;
					break;
				default:
					break;
				}
			}
	}
	float OldBonus = BonusValue;
	float CurrentBonus = BonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	CurrentBonus = (AdditiveBonus - SubtractBonus);
	CurrentBonus = (CurrentBonus * MultiplyBonus);
	CurrentBonus = (CurrentBonus / DivideBonus);
	//this is absolute maximum (not clamped right now).
	float addValue = CurrentBonus - OldBonus;
	//reset to max = 200
	Bonuses = FGAIndividualMods(AdditiveBonus, SubtractBonus, MultiplyBonus,
		DivideBonus, PercentageAddBonus, PercentageSubtractBonus);
	return CurrentValue + addValue;
}
float FGAAttributeBase::GetFinalValueByTags(const FGameplayTagContainer& TagsIn, FGAIndividualMods& Bonuses) const
{
	SCOPE_CYCLE_COUNTER(STAT_FinalBonusByTag);
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 1;
	float DivideBonus = 1;
	float PercentageAddBonus = 0;
	float PercentageSubtractBonus = 0;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		const FGAModifier& mod = ModIt->Value;
			if (ModIt->Key.HasAllTags(TagsIn))//mod.Handle->HasAllTags(TagsIn))
			{
				switch (mod.AttributeMod)
				{
				case EGAAttributeMod::Add:
					AdditiveBonus += mod.Value;
					break;
				case EGAAttributeMod::Subtract:
					SubtractBonus += mod.Value;
					break;
				case EGAAttributeMod::Multiply:
					MultiplyBonus += mod.Value;
					break;
				case EGAAttributeMod::Divide:
					DivideBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageAdd:
					PercentageAddBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageSubtract:
					PercentageSubtractBonus += mod.Value;
					break;
				default:
					break;
				}
			}
	}
	float OldBonus = BonusValue;
	float CurrentBonus = BonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	CurrentBonus = (AdditiveBonus - SubtractBonus);
	CurrentBonus = (CurrentBonus * MultiplyBonus);
	CurrentBonus = (CurrentBonus / DivideBonus);
	//this is absolute maximum (not clamped right now).
	float addValue = CurrentBonus - OldBonus;
	//reset to max = 200
	Bonuses = FGAIndividualMods(AdditiveBonus, SubtractBonus, MultiplyBonus,
		DivideBonus, PercentageAddBonus, PercentageSubtractBonus);
	return CurrentBonus + BaseValue;
}
float FGAAttributeBase::GetBonusValueByTags(const FGameplayTagContainer & TagsIn, FGAIndividualMods & Bonuses) const
{
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 1;
	float DivideBonus = 1;
	float PercentageAddBonus = 0;
	float PercentageSubtractBonus = 0;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		const FGAModifier& mod = ModIt->Value;
			if (ModIt->Key.HasAllTags(TagsIn))//mod.Handle->HasAllTags(TagsIn))
			{
				switch (mod.AttributeMod)
				{
				case EGAAttributeMod::Add:
					AdditiveBonus += mod.Value;
					break;
				case EGAAttributeMod::Subtract:
					SubtractBonus += mod.Value;
					break;
				case EGAAttributeMod::Multiply:
					MultiplyBonus += mod.Value;
					break;
				case EGAAttributeMod::Divide:
					DivideBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageAdd:
					PercentageAddBonus += mod.Value;
					break;
				case EGAAttributeMod::PercentageSubtract:
					PercentageSubtractBonus += mod.Value;
					break;
				default:
					break;
				}
			}
	}
	float CurrentBonus;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	CurrentBonus = (AdditiveBonus - SubtractBonus);
	CurrentBonus = (CurrentBonus * MultiplyBonus);
	CurrentBonus = (CurrentBonus / DivideBonus);
	Bonuses = FGAIndividualMods(AdditiveBonus, SubtractBonus, MultiplyBonus,
		DivideBonus, PercentageAddBonus, PercentageSubtractBonus);
	return CurrentBonus;
}
void FGAAttributeBase::UpdateAttribute()
{

}
void FGAAttributeBase::Add(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: OldCurrentValue: %f"), OldCurrentValue);
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: AddValue: %f"), ValueIn);
	float Val = CurrentValue - (OldCurrentValue + ValueIn);
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: ActuallAddVal: %f"), Val);
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Add:: CurrentValue: %f"), CurrentValue);
}
void FGAAttributeBase::Subtract(float ValueIn)
{
	float OldCurrentValue = CurrentValue;
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: OldCurrentValue: %f"), OldCurrentValue);
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: SubtractValue: %f"), ValueIn);
	float Val = CurrentValue - (OldCurrentValue - ValueIn);
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: ActuallSubtractVal: %f"), Val);
	CurrentValue -= Val;
	CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
	UE_LOG(GameAttributes, Log, TEXT("FGAAttributeBase::Subtract:: CurrentValue: %f"), CurrentValue);
}

void FGAAttributeBase::InitializeAttribute()
{
	BonusValue = 0;
	CalculateBonus();
	CurrentValue = GetFinalValue();
	//CurrentValue = BaseValue;
	
}