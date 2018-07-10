// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "GameplayTagContainer.h"
#include "../AFAbilityComponent.h"
#include "GAAttributesBase.h"
#include "../AFAbilityInterface.h"
#include "GAAttributeExtension.h"

#include "GAAttributeBase.h"
DEFINE_STAT(STAT_CalculateBonus);
DEFINE_STAT(STAT_CurrentBonusByTag);
DEFINE_STAT(STAT_FinalBonusByTag);
//UGAAttributeBase::UGAAttributeBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//
//}
FAFAttributeBase::FAFAttributeBase()
	: BaseBonusValue(0)
	, CurrentValue(0)
{
	Modifiers.AddDefaulted(7);
};
FAFAttributeBase::FAFAttributeBase(float BaseValueIn)
	: BaseValue(BaseValueIn)
	, BaseBonusValue(0)
	, CurrentValue(BaseValue)
	
{
	Modifiers.AddDefaulted(7);
};


void FAFAttributeBase::InitializeAttribute(UAFAbilityComponent* InComponent, const FName InAttributeName)
{
	CurrentValue = BaseValue;
	CalculateBonus();
	CurrentValue = GetFinalValue();
	Modifiers.Empty();
	Modifiers.AddDefaulted(7);// static_cast<int32>(EGAAttributeMod::Invalid));
	//Modifiers.AddDefaulted(static_cast<int32>(EGAAttributeMod::Invalid));
	AbilityComp = InComponent;
	SelfName = FGAAttribute(InAttributeName);
}
void FAFAttributeBase::CopyFromOther(FAFAttributeBase* Other)
{
	if (!Other)
		return;

	BaseValue = Other->BaseValue;
	MinValue = Other->MinValue;
	MaxValue = Other->MaxValue;
	CurrentValue = Other->CurrentValue;
	BaseBonusValue = Other->BaseBonusValue;

	CurrentValue = BaseValue;
	CalculateBonus();
	CurrentValue = GetFinalValue();
}
void FAFAttributeBase::CalculateBonus()
{
	SCOPE_CYCLE_COUNTER(STAT_CalculateBonus);
	float AdditiveBonus = 0;
	float SubtractBonus = 0;
	float MultiplyBonus = 1;
	float DivideBonus = 1;
	//auto ModIt = Modifiers.CreateConstIterator();
	TMap<FGAEffectHandle, FGAEffectMod>& Additive = Modifiers[static_cast<int32>(EGAAttributeMod::Add)];
	TMap<FGAEffectHandle, FGAEffectMod>& Subtractive = Modifiers[static_cast<int32>(EGAAttributeMod::Subtract)];
	TMap<FGAEffectHandle, FGAEffectMod>& Multiplicative = Modifiers[static_cast<int32>(EGAAttributeMod::Multiply)];
	TMap<FGAEffectHandle, FGAEffectMod>& Divide = Modifiers[static_cast<int32>(EGAAttributeMod::Divide)];
	for (auto ModIt = Additive.CreateConstIterator(); ModIt; ++ModIt)
	{
		AdditiveBonus += ModIt->Value.Value;
	}
	for (auto ModIt = Subtractive.CreateConstIterator(); ModIt; ++ModIt)
	{
		SubtractBonus += ModIt->Value.Value;
	}
	for (auto ModIt = Multiplicative.CreateConstIterator(); ModIt; ++ModIt)
	{
		MultiplyBonus += ModIt->Value.Value;
	}
	for (auto ModIt = Divide.CreateConstIterator(); ModIt; ++ModIt)
	{
		DivideBonus += ModIt->Value.Value;
	}

	float OldBonus = BaseBonusValue;
	//calculate final bonus from modifiers values.
	//we don't handle stacking here. It's checked and handled before effect is added.
	BaseBonusValue = (AdditiveBonus - SubtractBonus);
	BaseBonusValue = (BaseBonusValue * MultiplyBonus);
	BaseBonusValue = (BaseBonusValue / DivideBonus);
	//this is absolute maximum (not clamped right now).
	float addValue = BaseBonusValue - OldBonus;
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

bool FAFAttributeBase::CheckIfStronger(const FGAEffectMod& InMod)
{
	TMap<FGAEffectHandle, FGAEffectMod>& mods = Modifiers[static_cast<int32>(InMod.AttributeMod)];
	auto It = mods.CreateConstIterator();
	for (; It; ++It)
	{
		if (InMod > It->Value)
		{
			return true;
		}
	}
	if (mods.Num() <= 0)
	{
		return true;
	}
	return false;
}
float FAFAttributeBase::Modify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
	FGAEffectProperty& InProperty, const FGAEffectContext& InContext)
{
	//FString name = GetTypeName<FAFAttributeBase>();
	if (ExtensionClass)
	{
		ExtensionClass.GetDefaultObject()->OnPreAttributeModify(AbilityComp, SelfName, CurrentValue);
		
		ExtensionClass.GetDefaultObject()->PreAttributeModify(InContext
			, CurrentValue);
		
		
	}
	float returnValue = -1;
	bool isPeriod = InProperty.GetPeriod() > 0;
	bool IsDuration = InProperty.GetDuration() > 0;

	float PreValue = CurrentValue;
	
	if ( !InProperty.GetIsInstant())
	{
		FGAModifier AttrMod(ModIn.AttributeMod, ModIn.Value, HandleIn);
		AttrMod.Tags.AppendTags(InProperty.GetSpecData()->AttributeTags);
		AddBonus(ModIn, HandleIn);
		return ModIn.Value;
	}
	else
	{
		switch (ModIn.AttributeMod)
		{
		case EGAAttributeMod::Add:
		{
			float OldCurrentValue = CurrentValue;
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Add:: OldCurrentValue: %f"), OldCurrentValue);
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Add:: AddValue: %f"), ModIn.Value);
			float Val = CurrentValue - (OldCurrentValue + ModIn.Value);
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Add:: ActuallAddVal: %f"), Val);
			CurrentValue -= Val;
			CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Add:: CurrentValue: %f"), CurrentValue);
			returnValue = CurrentValue;
			break;
		}
		case EGAAttributeMod::Subtract:
		{
			float OldCurrentValue = CurrentValue;
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Subtract:: OldCurrentValue: %f"), OldCurrentValue);
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Subtract:: SubtractValue: %f"), ModIn.Value);
			float Val = CurrentValue - (OldCurrentValue - ModIn.Value);
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Subtract:: ActuallSubtractVal: %f"), Val);
			CurrentValue -= Val;
			CurrentValue = FMath::Clamp<float>(CurrentValue, 0, GetFinalValue());
			UE_LOG(GameAttributes, Log, TEXT("FAFAttributeBase::Subtract:: CurrentValue: %f"), CurrentValue);

			returnValue = CurrentValue;
			break;
		}
		case EGAAttributeMod::Multiply:
		{
			returnValue = -1;
			break;
		}
		case EGAAttributeMod::Divide:
		{
			returnValue = -1;
			break;
		}
		}
	}
	if (ExtensionClass)
	{
		ExtensionClass.GetDefaultObject()->OnPostAttributeModify(AbilityComp, SelfName, returnValue);
		
		ExtensionClass.GetDefaultObject()->PostAttributeModify(
			InContext
			, PreValue
			, returnValue);
		
	}
	return returnValue;
}

void FAFAttributeBase::AddBonus(const FGAEffectMod& ModIn, const FGAEffectHandle& Handle)
{
	TMap<FGAEffectHandle, FGAEffectMod>& mods = Modifiers[static_cast<int32>(ModIn.AttributeMod)];
	FGAEffectMod& modsTemp = mods.FindOrAdd(Handle);
	modsTemp = ModIn;
	CalculateBonus();
}
void FAFAttributeBase::RemoveBonus(const FGAEffectHandle& Handle, EGAAttributeMod InMod)
{
	TMap<FGAEffectHandle, FGAEffectMod>& mods = Modifiers[static_cast<int32>(InMod)];
	mods.Remove(Handle);
	//Modifiers.Remove(Handle);
	CalculateBonus();
}