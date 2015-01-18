// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GESEffect.h"
#include "GSAbility.h"

AGSAbility::AGSAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	OnSetWeaponsForAbility.AddUObject(this, &AGSAbility::SetWeaponForAbility);
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
void AGSAbility::OnRep_CastStarted()
{
	Super::OnRep_CastStarted();
	OnCueActionStarted.Broadcast();
}
void AGSAbility::OnRep_CastEnded()
{
	Super::OnRep_CastEnded();
	OnCueActionEnded.Broadcast();
}

void AGSAbility::SetWeaponForAbility(class UGSItemWeaponInfo* LeftWeaponIn, class UGSItemWeaponInfo* RightWeaponIn)
{
	CurrentLeftWeapon = LeftWeaponIn;
	CurrentRightWeapon = RightWeaponIn;
}