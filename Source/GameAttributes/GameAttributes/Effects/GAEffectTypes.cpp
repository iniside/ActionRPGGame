// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffectComponent.h"
#include "GAEffect.h"
#include "IGAEffect.h"
#include "../GAAttributeComponent.h"
#include "../IGAAttributes.h"

#include "GAEffectTypes.h"

bool FGAEffectAttributeSpec::InitialzeSpecs(const FGAttributeContext& Context)
{
	//would be nice to cache it.
	//but we need to refresh it on every application
	//because target could change.
	for (FGAAttributeSpec& spec : PeriodicAttributeSpec)
	{
		spec.AttributeMagnitude.AttributeContext = Context;
	}
	for (FGAAttributeSpec& spec : InitialAttributeSpec)
	{
		spec.AttributeMagnitude.AttributeContext = Context;
	}
	for (FGAAttributeSpec& spec : EndAttributeSpec)
	{
		spec.AttributeMagnitude.AttributeContext = Context;
	}

	return true;
}
void FGAEffectDuration::InitializeDuration(const FGAttributeContext& Context)
{
	DurationMagnitude.AttributeMagnitude.AttributeContext = Context;
	PeriodMagnitude.AttributeMagnitude.AttributeContext = Context;
}
void FGAEffectAttributeMods::SetAttributeData(const FGAEffectAttributeSpec& SpecIn)
{
	AttributeDataInitial.AttributeModSpec = SpecIn.InitialAttributeSpec;
	AttributeDataPeriodic.AttributeModSpec = SpecIn.PeriodicAttributeSpec;
	AttributeDataEnd.AttributeModSpec = SpecIn.EndAttributeSpec;
}
void FGAEffectAttributeMods::InitializeAttributeMods(const FGAttributeContext& Context, const FHitResult& Target)
{
	AttributeDataInitial.AttributeContext = Context;
	AttributeDataInitial.HitLocation = Target.Location;
	AttributeDataInitial.InitializeModData();

	AttributeDataPeriodic.AttributeContext = Context;
	AttributeDataPeriodic.HitLocation = Target.Location;
	AttributeDataPeriodic.InitializeModData();

	AttributeDataEnd.AttributeContext = Context;
	AttributeDataEnd.HitLocation = Target.Location;
	AttributeDataEnd.InitializeModData();
}
void FGAEffectSpec::SetAttributeData(const FGAEffectAttributeSpec& SpecIn)
{
	AttributeMods.SetAttributeData(SpecIn);
}

void FGAEffectSpec::ApplyInitialMods()
{
	AttributeMods.AttributeDataInitial.ApplyMod();
}
void FGAEffectSpec::ApplyPeriodicMods()
{
	AttributeMods.AttributeDataPeriodic.ApplyMod();
}
void FGAEffectSpec::ApplyEndMods()
{
	AttributeMods.AttributeDataEnd.ApplyMod();
}
float FGAEffectSpec::GetDuration()
{
	return Duration.DurationMagnitude.GetCurrentMagnitude();
}
float FGAEffectSpec::GetPeriod()
{
	return Duration.PeriodMagnitude.GetCurrentMagnitude();
}

bool FGAEffectSpec::InitializeEffectSpecs()
{
	bool bIsSpecInitialized = false;
	IIGAAttributes* instAttr = Cast<IIGAAttributes>(Instigator);
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.Actor.Get());

	if (instAttr && targetAttr)
	{
		FGAttributeContext AttrCtx;
		AttrCtx.Target = targetAttr->GetAttributeComponent();
		AttrCtx.Instigator = instAttr->GetAttributeComponent();
		AttributeMods.InitializeAttributeMods(AttrCtx, Target);
		Duration.InitializeDuration(AttrCtx);
		bIsSpecInitialized = true;
	}
	return bIsSpecInitialized;
}
UGAEffect* FGAEffectSpec::CreateEffect()
{
	UGAEffect* eff = nullptr;
	if (EffectClass)
	{
		eff = ConstructObject<UGAEffect>(EffectClass);
		if (eff)
		{
			//eff->Target = Target;
			//eff->Causer = Causer;
			//eff->Instigator = Instigator;
			//eff->InstigatorEffectComponent = InstigatorEffectComp;
			//eff->TargetEffectComponent = TargetEffectComp;
			//eff->MyTag = EffectTag;
			//eff->MagnitudeTags = MagnitudeTags;
			//eff->Magnitude = Magnitude;
			//eff->Duration = Duration;
			//eff->PeriodCount = PeriodCount;
		}
	}
	return eff;
}