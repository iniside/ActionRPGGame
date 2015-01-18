// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "DrawDebugHelpers.h"
#include "../GWWeapon.h"
#include "GWTraceBase_LineSingle.h"

UGWTraceBase_LineSingle::UGWTraceBase_LineSingle(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
void UGWTraceBase_LineSingle::TraceLineSingle()
{
	FHitResult Impact;
	GetOuterAGWWeapon()->TargetData.Empty();
	const FVector ShootDir = GetPawnCameraAim();

	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromTargetingSocket();
		const FVector EndTrace = (GetStartLocationFromTargetingSocket() + ShootDir * Range);
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

void UGWTraceBase_LineSingle::Execute()
{
	TraceLineSingle();
	PostExecute();
}