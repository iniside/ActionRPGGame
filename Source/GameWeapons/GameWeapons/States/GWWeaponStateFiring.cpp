// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "../GWWeapon.h"
#include "../GWWeaponRanged.h"
#include "GWWeaponStateFiring.h"

UGWWeaponStateFiring::UGWWeaponStateFiring(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UGWWeaponStateFiring::Tick(float DeltaSeconds)
{
}
void UGWWeaponStateFiring::BeginState(UGWWeaponState* PrevState)
{
	CurrentWeapon = Cast<AGWWeaponRanged>(GetOuterAGWWeapon());
	if (CurrentWeapon)
	{
		GetOuterAGWWeapon()->GetWorldTimerManager().SetTimer(FiringTimerHandle, this, &UGWWeaponStateFiring::FireWeapon, CurrentWeapon->FireRate, true);
		FireWeapon();
	}
}
void UGWWeaponStateFiring::EndState()
{
	GetOuterAGWWeapon()->GetWorldTimerManager().ClearTimer(FiringTimerHandle);
}
void UGWWeaponStateFiring::BeginActionSequence()
{
}
void UGWWeaponStateFiring::EndActionSequence()
{
	GetOuterAGWWeapon()->GotoState(GetOuterAGWWeapon()->ActiveState);
}

void UGWWeaponStateFiring::FireWeapon()
{
	CurrentWeapon->ActionBegin();
}