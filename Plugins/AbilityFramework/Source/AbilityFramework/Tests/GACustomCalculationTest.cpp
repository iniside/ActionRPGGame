// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "GAAttributesTest.h"
#include "GACustomCalculationTest.h"




float UGACustomCalculationTest::NativeCalculateMagnitude(const FGAEffectHandle& HandleIn)
{
	UGAAttributesTest* InstAttr = Cast<UGAAttributesTest>(HandleIn.GetContext().GetInstigatorAttributes());
	UGAAttributesTest* TargetAttr = Cast<UGAAttributesTest>(HandleIn.GetContext().GetTargetAttributes());
	if (InstAttr && TargetAttr)
	{
		FGAIndividualMods ModsOut;
		/*float MagicBonus = InstAttr->MagicalBonus.GetFinalValueByTags(HandleIn.GetOwnedTags(), ModsOut);
		float MagicResistance = TargetAttr->MagicResistance.GetFinalValueByTags(HandleIn.GetOwnedTags(), ModsOut);
		return MagicBonus - MagicResistance;*/
	}
	return 0;
}