// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "../GWWeapon.h"
#include "../GWWeaponRanged.h"

#include "GWWeaponStateSingleShot.h"

UGWWeaponStateSingleShot::UGWWeaponStateSingleShot(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bCanRefire = true;
}

void UGWWeaponStateSingleShot::Tick(float DeltaSeconds)
{
}
void UGWWeaponStateSingleShot::BeginState(UGWWeaponState* PrevState)
{
	CurrentWeapon = Cast<AGWWeaponRanged>(GetOuterAGWWeapon());
	if (CurrentWeapon)
	{
		FireWeapon();
		bCanRefire = false;
		CurrentWeapon->GetWorldTimerManager().SetTimer(CanRefireTimerHandle, this, &UGWWeaponStateSingleShot::SetCanRefireWeapon, CurrentWeapon->ReloadTime, false);
	}
}
void UGWWeaponStateSingleShot::EndState()
{
}
void UGWWeaponStateSingleShot::BeginActionSequence()
{
	if (bCanRefire)
	{
		bCanRefire = false;
		CurrentWeapon->ShootWeapon();
		CurrentWeapon->GetWorldTimerManager().SetTimer(CanRefireTimerHandle, this, &UGWWeaponStateSingleShot::SetCanRefireWeapon, CurrentWeapon->ReloadTime, false);
	}
}
void UGWWeaponStateSingleShot::EndActionSequence()
{
	GetOuterAGWWeapon()->GotoState(GetOuterAGWWeapon()->ActiveState);
}

void UGWWeaponStateSingleShot::FireWeapon()
{
	CurrentWeapon->GetWorldTimerManager().ClearTimer(CanRefireTimerHandle);
	CurrentWeapon->ShootWeapon();
}
void UGWWeaponStateSingleShot::SetCanRefireWeapon()
{
	bCanRefire = true;
}