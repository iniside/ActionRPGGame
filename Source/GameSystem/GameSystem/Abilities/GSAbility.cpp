// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GESEffect.h"
#include "GSAbility.h"

AGSAbility::AGSAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


UGESEffect* AGSAbility::GetCachedEffect()
{
	return CachedEffect;
}

bool AGSAbility::CheckIfCanUseAbility()
{
	return true; //implement checking for attribute.
}

void AGSAbility::SubtractAttributeCost()
{

}