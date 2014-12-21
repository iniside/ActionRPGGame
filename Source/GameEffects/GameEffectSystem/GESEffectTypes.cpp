// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameEffectSystem.h"
#include "GESEffectComponent.h"
#include "GESEffect.h"
#include "IGESEffect.h"
#include "GESEffectTypes.h"

FGESEffectSpec::FGESEffectSpec()
{

}

FGESEffectSpec::FGESEffectSpec(TWeakObjectPtr<AActor> TargetIn, TWeakObjectPtr<AActor> CauserIn,
	TWeakObjectPtr<AActor> InstigatorIn)
{
	IIGESEffect* targetInt = Cast<IIGESEffect>(TargetIn.Get());
	IIGESEffect* instigatorInt = Cast<IIGESEffect>(InstigatorIn.Get());

	TargetComponent = targetInt->GetEffectComponent();
	InstigatorComponent = instigatorInt->GetEffectComponent();
}