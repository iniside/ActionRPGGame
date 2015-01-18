// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "../GWWeapon.h"
#include "../GWWeaponRanged.h"
#include "GWWeaponStateActiveReload.h"

UGWWeaponStateActiveReload::UGWWeaponStateActiveReload(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UGWWeaponStateActiveReload::Tick(float DeltaSeconds)
{
}
void UGWWeaponStateActiveReload::BeginState(UGWWeaponState* PrevState)
{
	CurrentWeapon = Cast<AGWWeaponRanged>(GetOuterAGWWeapon());
	if (CurrentWeapon)
	{
		CurrentWeapon->GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &UGWWeaponStateActiveReload::EndWeaponReload, CurrentWeapon->ReloadTime, false);
		//CurrentWeapon->BeginReload();
	}
}
void UGWWeaponStateActiveReload::EndState()
{
}
void UGWWeaponStateActiveReload::BeginActionSequence()
{
}
void UGWWeaponStateActiveReload::EndActionSequence()
{
}
void UGWWeaponStateActiveReload::EndWeaponReload()
{
	CurrentWeapon->GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	CurrentWeapon->EndReload();
	CurrentWeapon->GotoState(CurrentWeapon->ActiveState);
}