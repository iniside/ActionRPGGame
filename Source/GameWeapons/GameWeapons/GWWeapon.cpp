// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "States/GWWeaponState.h"
#include "GWWeapon.h"

AGWWeapon::AGWWeapon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	RootMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("RootMesh"));
	RootComponent = RootMesh;
	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh"));
	WeaponMesh->AttachParent = RootComponent;
}

void AGWWeapon::InitializeWeapon()
{

}

void AGWWeapon::InputPressed()
{

}
void AGWWeapon::InputReleased()
{

}

void AGWWeapon::BeginCustomization()
{

}
void AGWWeapon::FinishCustomization()
{

}

void AGWWeapon::GotoState(class UGWWeaponState* NextState)
{
	if (NextState == NULL || !NextState->IsIn(this))
	{

	}
	else if (CurrentState != NextState)
	{
		UGWWeaponState* PrevState = CurrentState;
		if (CurrentState != NULL)
		{
			CurrentState->EndState();
		}
		if (CurrentState == PrevState)
		{
			CurrentState = NextState;
			CurrentState->BeginState(PrevState);
		}
	}
}