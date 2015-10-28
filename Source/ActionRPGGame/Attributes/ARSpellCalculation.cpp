// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "GAEffects.h"
#include "ARCharacterAttributes.h"
#include "ARSpellCalculation.h"

#include "GAGameEffect.h"

UARSpellCalculation::UARSpellCalculation(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
FGAAttributeData UARSpellCalculation::OutgoingModifyEffect(const FGAAttributeData& DataIn)
{
	FGAAttributeData data = DataIn;
	return data;
}
FGAAttributeData UARSpellCalculation::IncomingModifyEffect(const FGAAttributeData& DataIn)
{
	FGAAttributeData data = DataIn;
	UARCharacterAttributes* InstAttr = Cast<UARCharacterAttributes>(DataIn.Context.InstigatorComp->DefaultAttributes);
	UARCharacterAttributes* TargAttr = Cast<UARCharacterAttributes>(DataIn.Context.TargetComp->DefaultAttributes);
	//UProperty* ModifiedProperty = FindFieldChecked<UProperty>(UARCharacterAttributes::StaticClass(), data.Attribute.AttributeName);

	FGAModifierStack OffStack = DataIn.Context.InstigatorComp->ActiveEffects.ModifierContainer.GetOutgoingModifierStack(DataIn);
	FGAModifierStack DefStack = DataIn.Context.TargetComp->ActiveEffects.ModifierContainer.GetIncomingModifierStack(DataIn);
	//
	
	data.Value = data.Value + OffStack.Additive;
	data.Value = data.Value - OffStack.Subtractive;

	data.Value = data.Value + (data.Value * OffStack.Multiply);
	data.Value = data.Value - (data.Value * OffStack.Divide);

	//so additive bonus in defense would work like penalty and increas damage received ?
	data.Value = data.Value + DefStack.Additive;
	data.Value = data.Value - DefStack.Subtractive;

	data.Value = data.Value + (data.Value * DefStack.Multiply);
	data.Value = data.Value - (data.Value * DefStack.Divide);
	
	return data;
}

FGAGameEffect UARSpellCalculation::ModiifyEffect(const FGAGameEffect& EffectIn)
{
	FGAGameEffect EffectOut = EffectIn;
	//FGAGameModifierStack InstigatorStack = EffectOut.GetInstigatorComp()->GameEffectContainer.GetQualifableMods(EffectIn, EGAModifierDirection::Outgoing);
	//FGAGameModifierStack TargetStack = EffectOut.GetTargetComp()->GameEffectContainer.GetQualifableMods(EffectIn, EGAModifierDirection::Incoming);
	//
	//EffectOut.Value = EffectOut.Value + InstigatorStack.Additive;
	//EffectOut.Value = EffectOut.Value - InstigatorStack.Subtractive;

	//EffectOut.Value = EffectOut.Value + (EffectOut.Value * InstigatorStack.Multiply);
	//EffectOut.Value = EffectOut.Value - (EffectOut.Value * InstigatorStack.Divide);

	////so additive bonus in defense would work like penalty and increas damage received ?
	//EffectOut.Value = EffectOut.Value + TargetStack.Additive;
	//EffectOut.Value = EffectOut.Value - TargetStack.Subtractive;

	//EffectOut.Value = EffectOut.Value + (EffectOut.Value * TargetStack.Multiply);
	//EffectOut.Value = EffectOut.Value - (EffectOut.Value * TargetStack.Divide);

	return EffectOut;
}

FGAGameEffect UARSpellCalculation::ModifyEffectTarget(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context)
{
	FGAGameEffect EffectOut = EffectIn;
	FGAGameModifierStack InstigatorStack = Context.GetModifiers(EffectIn, Context);

	Mod.Value = Mod.Value + InstigatorStack.Additive;
	Mod.Value = Mod.Value - InstigatorStack.Subtractive;

	Mod.Value = Mod.Value + (Mod.Value * InstigatorStack.Multiply);
	Mod.Value = Mod.Value - (Mod.Value * InstigatorStack.Divide);
	return EffectOut;
}

FGAGameEffect UARSpellCalculation::ModifyEffectInstigator(const FGAGameEffect& EffectIn, FGAGameEffectMod& Mod, FGACalculationContext& Context)
{
	FGAGameEffect EffectOut = EffectIn;
	FGAGameModifierStack InstigatorStack = Context.GetModifiers(EffectIn, Context);

	Mod.Value = Mod.Value + InstigatorStack.Additive;
	Mod.Value = Mod.Value - InstigatorStack.Subtractive;

	Mod.Value = Mod.Value + (Mod.Value * InstigatorStack.Multiply);
	Mod.Value = Mod.Value - (Mod.Value * InstigatorStack.Divide);
	return EffectOut;
}

void UARSpellCalculation::ModifyEffect(const FGAGameEffect& EffectIn, FGAEffectMod& ModIn, FGACalculationContext& Context)
{
	FGAGameModifierStack InstigatorStack = Context.GetModifiers(EffectIn, Context);

	ModIn.Value = ModIn.Value + InstigatorStack.Additive;
	ModIn.Value = ModIn.Value - InstigatorStack.Subtractive;

	ModIn.Value = ModIn.Value + (ModIn.Value * InstigatorStack.Multiply);
	ModIn.Value = ModIn.Value - (ModIn.Value * InstigatorStack.Divide);
}