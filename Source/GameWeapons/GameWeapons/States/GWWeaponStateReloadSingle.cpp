// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "../GWWeapon.h"
#include "../GWWeaponRanged.h"
#include "GWWeaponStateReloadSingle.h"

UGWWeaponStateReloadSingle::UGWWeaponStateReloadSingle(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UGWWeaponStateReloadSingle::Tick(float DeltaSeconds)
{
}
void UGWWeaponStateReloadSingle::BeginState(UGWWeaponState* PrevState)
{
	CurrentWeapon = Cast<AGWWeaponRanged>(GetOuterAGWWeapon());
	if (CurrentWeapon)
	{
		//CurrentWeapon->GetWorldTimerManager().SetTimer(this, &UGWWeaponStateReloadSingle::EndWeaponReload, CurrentWeapon->ReloadTime, false);
		//CurrentWeapon->BeginReload();
	}
}
void UGWWeaponStateReloadSingle::EndState()
{
}
void UGWWeaponStateReloadSingle::BeginActionSequence()
{
}
void UGWWeaponStateReloadSingle::EndActionSequence()
{
}
void UGWWeaponStateReloadSingle::EndWeaponReload()
{
	CurrentWeapon->EndReload();
	CurrentWeapon->GotoState(CurrentWeapon->ActiveState);
}