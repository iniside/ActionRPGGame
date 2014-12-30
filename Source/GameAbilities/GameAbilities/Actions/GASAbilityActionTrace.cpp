// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "DrawDebugHelpers.h"
#include "GASAbilityActionTrace.h"

UGASAbilityActionTrace::UGASAbilityActionTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}

void UGASAbilityActionTrace::Tick(float DeltaSecondsIn)
{

}

FVector UGASAbilityActionTrace::SingleLineTrace()
{
	GetOuterAGASAbility()->TargetData.Empty();
	const FVector ShootDir = GetPawnCameraAim();
	FHitResult Impact;
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (GetPawnCameraDamageStartLocation(ShootDir) + ShootDir * GetOuterAGASAbility()->AbilityRange);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * GetOuterAGASAbility()->AbilityRange);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	GetOuterAGASAbility()->TargetData.Add(Impact);

	return Impact.Location;
}
void UGASAbilityActionTrace::PreExecute()
{
	//display something. Like spawn targeting actor circle. Or sphere. Or whatever.
}
void UGASAbilityActionTrace::Execute()
{
	SingleLineTrace();
}
FVector UGASAbilityActionTrace::GetPawnCameraAim()
{
	FVector FinalAim = FVector::ZeroVector;

	if (GetOuterAGASAbility()->PawnOwner)
	{
		FinalAim = GetOuterAGASAbility()->PawnOwner->GetBaseAimRotation().Vector();
	}
	return FinalAim;
}

FVector UGASAbilityActionTrace::GetPCCameraAim()
{
	FVector OutStartTrace = FVector::ZeroVector;
	FRotator AimDir;
	if (GetOuterAGASAbility()->PCOwner)
	{
		GetOuterAGASAbility()->PCOwner->GetPlayerViewPoint(OutStartTrace, AimDir);
	}
	return AimDir.Vector();
}

FVector UGASAbilityActionTrace::GetStartLocationFromSocket()
{
	return GetOuterAGASAbility()->GetSocketLocation(StartLocation);
}

FVector UGASAbilityActionTrace::GetPawnCameraDamageStartLocation(const FVector& AimDir)
{
	FVector OutStartTrace = FVector::ZeroVector;
	if (GetOuterAGASAbility()->PawnOwner)
	{
		FRotator UnusedRot;
		if (GetOuterAGASAbility()->PCOwner)
		{
			GetOuterAGASAbility()->PCOwner->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		}
		FVector ActorLocation = GetOuterAGASAbility()->PawnOwner->GetActorLocation();
		OutStartTrace = OutStartTrace + AimDir * ((ActorLocation - OutStartTrace) | AimDir);
		
		//OutStartTrace = OutStartTrace + AimDir * ((GetOuterAGASAbility()->PawnOwner->GetActorLocation() - OutStartTrace));
		return OutStartTrace;
	}
	return OutStartTrace;
}

FHitResult UGASAbilityActionTrace::SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FHitResult Hit(ForceInit);
	UWorld* world = GetWorld();
	if (!GetOuterAGASAbility()->PawnOwner)
		return Hit;

	static FName PowerTag = FName(TEXT("SingleLineTrace"));
	FCollisionQueryParams TraceParams(PowerTag, false, GetOuterAGASAbility()->PawnOwner);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = true;
	
	if (bIgnoreSelf)
	{
		TraceParams.AddIgnoredActor(GetOuterAGASAbility()->PawnOwner);
	}

	FCollisionObjectQueryParams ObjectParams;
	for (auto Iter = ObjectsToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			ObjectParams.AddObjectTypesToQuery(Channel);
		}
	}

	bool traceResult = GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, TraceParams, ObjectParams);
	if (bDrawDebug)
	{
		if (traceResult && Hit.bBlockingHit)
		{
			::DrawDebugLine(world, StartTrace, Hit.ImpactPoint, FColor::Red, false, 2);
			::DrawDebugLine(world, Hit.ImpactPoint, EndTrace, FColor::Green, false, 2);
			::DrawDebugPoint(world, Hit.ImpactPoint, 7, FColor::Red, false, 2);
		}
		else
		{
			::DrawDebugPoint(world, Hit.ImpactPoint, 15, FColor::Red, false, 2);
		}
	}
	return Hit;
}
