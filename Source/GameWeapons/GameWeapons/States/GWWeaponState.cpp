// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "../GWWeapon.h"
#include "GWWeaponState.h"

UGWWeaponState::UGWWeaponState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGWWeaponState::GetWorld() const
{
	return GetOuterAGWWeapon()->GetWorld();
}

void UGWWeaponState::Tick(float DeltaSeconds)
{
}
void UGWWeaponState::BeginState(UGWWeaponState* PrevState)
{
}
void UGWWeaponState::EndState()
{
}
void UGWWeaponState::BeginActionSequence()
{
}
void UGWWeaponState::EndActionSequence()
{
}