// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"

#include "GAAttributeBase.h"

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
	case EGAAttributeMod::Multiply:
		CurrentValue *= ModifierIn.Value;
		break;
	case EGAAttributeMod::Divide:
		CurrentValue /= ModifierIn.Value;
		break;
	default:
		break;
	}
}
float FGAAttributeBase::Modify(const FGAEffectMod& ModIn)
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
	return -1;
}
void FGAAttributeBase::AddBonus(const FGAModifier& ModifiersIn, const FGAGameEffectHandle& Handle
	, EGAEffectStacking StackingType)
{
	switch (StackingType)
	{
	case EGAEffectStacking::Override:
		RemoveBonusByType(ModifiersIn.AttributeMod);
		break;
	case EGAEffectStacking::StrongerOverride:
		RemoveWeakerBonus(ModifiersIn.AttributeMod, ModifiersIn.Value);
		break;
	}

	TArray<FGAModifier>& modsTemp = Modifiers.FindOrAdd(Handle);
	modsTemp.Add(ModifiersIn);
	CalculateBonus();
}
void FGAAttributeBase::RemoveBonus(const FGAGameEffectHandle& Handle)
{
	Modifiers.Remove(Handle);
	CalculateBonus();
}

void FGAAttributeBase::RemoveBonusByType(EGAAttributeMod ModType)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		for (auto MIt = It->Value.CreateIterator(); It; ++It)
		{
			//check if current attribute mod have the same mod
			//and is smaller than the the incoming one.
			if (MIt->AttributeMod == ModType)
			{
				//and remove it.
				It->Value.RemoveAt(MIt.GetIndex());
				//if there is no more mods, for this key, just remove it from map.
				if (It->Value.Num() <= 0)
				{
					Modifiers.Remove(It.Key());
				}
			}
		}
	}
	CalculateBonus();
}
void FGAAttributeBase::RemoveWeakerBonus(EGAAttributeMod ModType, float ValueIn)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		for (auto MIt = It->Value.CreateIterator(); It; ++It)
		{
			//check if current attribute mod have the same mod
			//and is smaller than the the incoming one.
			if (MIt->AttributeMod == ModType
				&& MIt->Value <= ValueIn)
			{
				//and remove it.
				It->Value.RemoveAt(MIt.GetIndex());
				//if there is no more mods, for this key, just remove it from map.
				if (It->Value.Num() <= 0)
				{
					Modifiers.Remove(It.Key());
				}
			}
		}
	}
	CalculateBonus();
}

void FGAAttributeBase::RemoveBonusType(EGAAttributeMod ModType)
{
	for (auto It = Modifiers.CreateIterator(); It; ++It)
	{
		for (auto MIt = It->Value.CreateIterator(); It; ++It)
		{
			//check if current attribute mod have the same mod
			if (MIt->AttributeMod == ModType)
			{
				//and remove it.
				It->Value.RemoveAt(MIt.GetIndex());
				//if there is no more mods, for this key, just remove it from map.
				if (It->Value.Num() <= 0)
				{
					Modifiers.Remove(It.Key());
				}
			}
		}
	}
	CalculateBonus();
}

void FGAAttributeBase::CalculateBonus()
{
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 0;
	float DivideBonus = 0;
	auto ModIt = Modifiers.CreateConstIterator();
	for (ModIt; ModIt; ++ModIt)
	{
		for (const FGAModifier& mod : ModIt->Value)
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
			default:
				break;
			}
		}
	}
	float OldBonus = BonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	BonusValue = (AdditiveBonus - SubtractBonus);
	BonusValue = BonusValue + (BonusValue * MultiplyBonus);
	BonusValue = BonusValue - (BonusValue * DivideBonus);
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
	CurrentValue = GetFinalValue();
	CurrentValue = BaseValue;
	CalculateBonus();
}

float FGAModifierMagnitude::GetMagnitude(const FGAEffectContext& Context)
{
	FGAAttributeBase* attr = nullptr;
	float Result = 0;
	switch (Source)
	{
	case EGAAttributeSource::Instigator:
		attr = Context.InstigatorComp->GetAttribute(Attribute);
		break;
	case EGAAttributeSource::Target:
		attr = Context.TargetComp->GetAttribute(Attribute);
		break;
	default:
		return 0;
	}
	FString ContextString(TEXT("Evaluating Modifier value based on attribute."));
	Result = CurveTable.Eval(attr->GetFinalValue(), ContextString);
	return Result;
}