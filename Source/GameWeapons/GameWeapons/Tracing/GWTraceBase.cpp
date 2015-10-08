// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "DrawDebugHelpers.h"
#include "../GWWeapon.h"
#include "GWTraceBase.h"

UGWTraceBase::UGWTraceBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
UWorld* UGWTraceBase::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UGWTraceBase::BeginDestroy()
{
	//TraceInterface = nullptr;
	Super::BeginDestroy();
}
void UGWTraceBase::Tick(float DeltaSecondsIn)
{

}

void UGWTraceBase::SingleLineTraceSetHitLocation()
{
	const FVector ShootDir = GetPawnCameraAim();
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromTargetingSocket();
		const FVector EndTrace = (GetStartLocationFromTargetingSocket() + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		GetOuterAGWWeapon()->SetHitLocation(StartTrace, Impact.Location);
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		GetOuterAGWWeapon()->SetHitLocation(StartTrace, Impact.Location);
	}
}

FVector UGWTraceBase::GetSingHitLocation()
{
	FHitResult Impact;
	const FVector ShootDir = GetPawnCameraAim();
	//
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromTargetingSocket();
		const FVector EndTrace = (GetStartLocationFromTargetingSocket() + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	
	return Impact.Location;
}

void UGWTraceBase::SingleLineTraceFromWeapon()
{
	const FVector ShootDir = GetPawnCameraAim();
	const FVector StartTrace = GetStartLocationFromWeaponSocket();
	const FVector EndTrace = (GetStartLocationFromWeaponSocket() + ShootDir * Range);
	const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	GetOuterAGWWeapon()->SetHitLocation(StartTrace, Impact.Location);
}

void UGWTraceBase::Initialize()
{
	for (auto Iter = ObjectsToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			CollisionObjectParams.AddObjectTypesToQuery(Channel);
		}
	}
}

void UGWTraceBase::PreExecute()
{
	//display something. Like spawn targeting actor circle. Or sphere. Or whatever.
}
void UGWTraceBase::Execute()
{

}

void UGWTraceBase::PostExecute()
{

}

void UGWTraceBase::ModifyExtendsByPrecentage(float PrcentageIn)
{

}
void UGWTraceBase::BP_ModiyExtendsByPrcentage(float PrecentageIn)
{
	ModifyExtendsByPrecentage(PrecentageIn);
}
void UGWTraceBase::ModifyRangeByPrecentage(float PrcentageIn)
{

}

void UGWTraceBase::SetTraceRange(float PrcentageIn)
{

}

FVector UGWTraceBase::GetPawnCameraAim()
{
	FVector FinalAim = FVector::ZeroVector;

	if (GetOuterAGWWeapon()->Instigator)
	{
		FinalAim = GetOuterAGWWeapon()->Instigator->GetBaseAimRotation().Vector();
	}
	return FinalAim;
}

FVector UGWTraceBase::GetPCCameraAim()
{
	FVector OutStartTrace = FVector::ZeroVector;
	FRotator AimDir;
	//if (TraceInterface->GetPC())
	//{
	//	TraceInterface->GetPC()->GetPlayerViewPoint(OutStartTrace, AimDir);
	//}
	return AimDir.Vector();
}

FVector UGWTraceBase::GetStartLocationFromTargetingSocket()
{
	return GetOuterAGWWeapon()->GetTargetingSocketLocation();
}

FVector UGWTraceBase::GetStartLocationFromWeaponSocket()
{
	return GetOuterAGWWeapon()->GetWeaponSocketLocation();
}

FVector UGWTraceBase::GetPawnCameraDamageStartLocation(const FVector& AimDir)
{
	FVector OutStartTrace = FVector::ZeroVector;
	if (GetOuterAGWWeapon()->Instigator)
	{
		FRotator UnusedRot;
		if (GetOuterAGWWeapon()->WeaponPC)
		{
			GetOuterAGWWeapon()->WeaponPC->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		}
		FVector ActorLocation = GetOuterAGWWeapon()->Instigator->GetActorLocation();
		OutStartTrace = OutStartTrace + AimDir * ((ActorLocation - OutStartTrace) | AimDir);
		
		//OutStartTrace = OutStartTrace + AimDir * ((GetOuterAGASAbility()->PawnOwner->GetActorLocation() - OutStartTrace));
		return OutStartTrace;
	}
	return OutStartTrace;
}

FHitResult UGWTraceBase::SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FHitResult Hit(ForceInit);
	UWorld* world = GetWorld();
	if (!GetOuterAGWWeapon()->Instigator)
		return Hit;

	static FName PowerTag = FName(TEXT("SingleLineTrace"));
	FCollisionQueryParams TraceParams(PowerTag, false, GetOuterAGWWeapon()->Instigator);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = true;
	
	if (bIgnoreSelf)
	{
		TraceParams.AddIgnoredActor(GetOuterAGWWeapon()->Instigator);
	}

	bool traceResult = GetWorld()->LineTraceSingleByObjectType(Hit, StartTrace, EndTrace, CollisionObjectParams, TraceParams);
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


FVector UGWTraceBase::GetHelperScale()
{
	return FVector::ZeroVector;
}