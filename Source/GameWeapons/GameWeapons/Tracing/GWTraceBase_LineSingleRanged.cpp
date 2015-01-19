// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "DrawDebugHelpers.h"
#include "../GWWeapon.h"
#include "GWTraceBase_LineSingleRanged.h"

UGWTraceBase_LineSingleRanged::UGWTraceBase_LineSingleRanged(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
void UGWTraceBase_LineSingleRanged::TraceLineSingle()
{
	FHitResult Impact;
	GetOuterAGWWeapon()->TargetData.Empty();
	const FVector AimDir = GetPawnCameraAim();
	int32 CurrentSeed = 0;
	if (GetOuterAGWWeapon()->GetNetMode() == ENetMode::NM_Client
		|| GetOuterAGWWeapon()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		if (GetOuterAGWWeapon()->Instigator->PlayerState)
			CurrentSeed = GetOuterAGWWeapon()->Instigator->PlayerState->StartTime;
	}
	else if (GetOuterAGWWeapon()->GetNetMode() == ENetMode::NM_Standalone)
		CurrentSeed = FMath::Rand();

	float ConeHalfAngl = FMath::DegreesToRadians(CurrentSpreadRadius* 0.5f);
	FRandomStream RandomWeaponStream(CurrentSeed);
	const FVector ShootDir = RandomWeaponStream.VRandCone(AimDir, ConeHalfAngl,ConeHalfAngl);

	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromTargetingSocket();
		const FVector EndTrace = (GetStartLocationFromTargetingSocket() + ShootDir * Range);
		//const FVector StartTrace = GetStartLocationFromTargetingSocket();
		//const FVector EndTrace = (GetStartLocationFromTargetingSocket() + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//another trace this time from weapon, to impact point.
		if (Impact.bBlockingHit)
		{
			const FVector CorrectStart = GetStartLocationFromWeaponSocket();
			GetOuterAGWWeapon()->TargetData.Add(Impact);
			GetOuterAGWWeapon()->SetHitLocation(CorrectStart, Impact.ImpactPoint);
		}
		else if (!Impact.bBlockingHit)
		{
			GetOuterAGWWeapon()->SetHitLocation(StartTrace, EndTrace);
		}
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	
	//GetOuterAGWWeapon()->TargetData.Add(Impact);
}

void UGWTraceBase_LineSingleRanged::Execute()
{
	TraceLineSingle();
	PostExecute();
}