// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "States/GWWeaponState.h"

#include "Net/UnrealNetwork.h"

#include "IGISocket.h"

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
void AGWWeapon::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGWWeapon, HitInfo, COND_SkipOwner);
}
void AGWWeapon::InitializeWeapon()
{
	SocketInt = Cast<IIGISocket>(Instigator);
}
void AGWWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	if (!SocketInt)
		SocketInt = Cast<IIGISocket>(Instigator);
}
void AGWWeapon::InputPressed()
{

}
void AGWWeapon::InputReleased()
{

}

void AGWWeapon::ActionBegin()
{

}
void AGWWeapon::ActionEnd()
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

void AGWWeapon::SetHitLocation(FVector StartLocation, FVector ImpactLocation)
{
	HitInfo.HitCounter++; 
	HitInfo.Origin = StartLocation;
	HitInfo.HitLocation = ImpactLocation;
}
void AGWWeapon::OnRep_HitInfo()
{

}
FVector AGWWeapon::GetTargetingSocketLocation()
{
	if (SocketInt)
	{
		return SocketInt->GetSocketLocation(TargetingSocketName);
	}
	return FVector::ZeroVector;
}
FVector AGWWeapon::GetWeaponSocketLocation()
{
	if (WeaponMesh)
	{
		return WeaponMesh->GetSocketLocation(WeaponSocketName);
	}
	return FVector::ZeroVector;
}