// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "IGTTrace.h"
#include "GASTrace_SingleLineTrace.h"

UGASTrace_SingleLineTrace::UGASTrace_SingleLineTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
void UGASTrace_SingleLineTrace::Execute()
{
	TraceSingleLine();
}
void UGASTrace_SingleLineTrace::TraceSingleLine()
{
	GetOuterUGASAbility()->TargetData.Empty();
	const FVector AimDir = GetPawnCameraAim();
	FHitResult Impact;
	const FVector CorrectStart = GetOuterUGASAbility()->GetCorrectedSocketLocation(EffectOriginLocation);
	if (bTraceFromSocket)
	{
		const FVector socketLocation = GetStartLocationFromSocket();
		const FVector StartTrace = socketLocation;
		const FVector EndTrace = (socketLocation + AimDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//another trace this time from weapon, to impact point.
		if (Impact.bBlockingHit)
		{
			GetOuterUGASAbility()->TargetData.Add(Impact);
			GetOuterUGASAbility()->SetHitLocation(CorrectStart, Impact.ImpactPoint);
		}
		else if (!Impact.bBlockingHit)
		{
			GetOuterUGASAbility()->SetHitLocation(CorrectStart, EndTrace);
		}
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(AimDir);
		const FVector EndTrace = (StartTrace + AimDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		if (Impact.bBlockingHit)
		{
			GetOuterUGASAbility()->TargetData.Add(Impact);
			GetOuterUGASAbility()->SetHitLocation(CorrectStart, Impact.ImpactPoint);
		}
		else if (!Impact.bBlockingHit)
		{
			GetOuterUGASAbility()->SetHitLocation(CorrectStart, EndTrace);
		}
		if (bDrawDebug)
		{
			if (Impact.bBlockingHit)
			{
				::DrawDebugLine(GetWorld(), StartTrace, Impact.ImpactPoint, FColor::Red, false, 2);
				::DrawDebugLine(GetWorld(), Impact.ImpactPoint, EndTrace, FColor::Green, false, 2);
				::DrawDebugPoint(GetWorld(), Impact.ImpactPoint, 7, FColor::Red, false, 2);
			}
			else
			{
				::DrawDebugLine(GetWorld(), StartTrace, Impact.ImpactPoint, FColor::Red, false, 2);
				::DrawDebugPoint(GetWorld(), Impact.ImpactPoint, 15, FColor::Red, false, 2);
			}
		}
	}
}