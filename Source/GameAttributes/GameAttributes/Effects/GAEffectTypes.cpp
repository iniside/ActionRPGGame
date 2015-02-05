// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffectComponent.h"
#include "GAEffect.h"
#include "IGAEffect.h"
#include "GAEffectTypes.h"

//FGESEffectSpec::FGESEffectSpec()
//{
//
//}
//
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