// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"
#include "GAAbilitiesComponent.h"
#include "Attributes/GAAttributesBase.h"
#include "ARCharacterAttributes.h"
#include "ARSpellCalculation.h"

#include "Effects/GAGameEffect.h"

UARSpellCalculation::UARSpellCalculation(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
float UARSpellCalculation::NativeCalculateMagnitude(const FGAEffectHandle& HandleIn)
{
	UARCharacterAttributes* InstigatorAttr = Cast<UARCharacterAttributes>(HandleIn.GetContextRef().GetInstigatorAttributes());
	float FinalValue = 0;
	float Bonuses = 0;
	FGAEffect& Effect = HandleIn.GetEffectRef();
	if (InstigatorAttr)
	{
		//FinalValue = (InstigatorAttr->Intelligence.GetCurrentValue() * 1)
		//	+ InstigatorAttr->IntelligenceMod.GetCurrentValue()
		//	+ (InstigatorAttr->Magic.GetCurrentValue() * 1);
		//FGAIndividualMods ModsOut;
		//Bonuses = InstigatorAttr->MagicalDamage.GetBonusValueByTags(Effect.OwnedTags, ModsOut);
	}
	return FinalValue;
}