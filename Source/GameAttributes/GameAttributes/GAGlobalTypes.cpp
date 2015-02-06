#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAAttributeComponent.h"
#include "GAAttributesBase.h"
#include "GAGlobalTypes.h"

float FGAAttributeMagnitude::CalculateMagnitude()
{
	float Result = 0;
	//for (FGAAttributeMagnitudeSpec& spec : MagnitudeSpecs)
	//{
	FName AttributeValue = "-1";
	if (MagnitudeSpec.AttributeSource == EGAAttributeSource::Instigator)
	{
		AttributeValue = *FString::FormatAsNumber(AttributeContext.Instigator->DefaultAttributes->GetFloatValue(MagnitudeSpec.AttributeMod));
	}
	else if (MagnitudeSpec.AttributeSource == EGAAttributeSource::Target)
	{
		AttributeValue = *FString::FormatAsNumber(AttributeContext.Target->DefaultAttributes->GetFloatValue(MagnitudeSpec.AttributeMod));
	}
	if (MagnitudeSpec.MagnitudeTable)
	{
		//UCurveTable* curveTable;
		FString WtfIsThisString;
		FGAttributeMagnitudeData* mag = MagnitudeSpec.MagnitudeTable->FindRow<FGAttributeMagnitudeData>(AttributeValue, WtfIsThisString);
		if (mag)
		{
			Result = mag->Magnitude;
		}
	}
	//}
	return Result;
}

float FGAAttributeSpec::GetCurrentMagnitude()
{
	if (bUseMagnitude)
	{
		CalculatedMagnitude = AttributeMagnitude.CalculateMagnitude();
		return CalculatedMagnitude;
	}
	else
	{
		CalculatedMagnitude = ModValue;
		return CalculatedMagnitude;
	}
	return 0;
}
void FGAAttributeModData::InitializeModData()
{
	for (FGAAttributeSpec& spec : AttributeModSpec)
	{
		//if (spec.bUseMagnitude)
		//{
			spec.AttributeMagnitude.AttributeContext = AttributeContext;
		//}
	}
}

void FGAAttributeModData::CalculcateCurrentMods()
{
	for (FGAAttributeSpec& spec : AttributeModSpec)
	{
		spec.GetCurrentMagnitude();
	}
}

void FGAAttributeModData::ApplyMod()
{
	if (!AttributeContext.Instigator.IsValid())
		return;
	CalculcateCurrentMods();

	AttributeContext.Instigator->ModifyAttributesOnTarget(AttributeContext.Target.Get(), *this);
}