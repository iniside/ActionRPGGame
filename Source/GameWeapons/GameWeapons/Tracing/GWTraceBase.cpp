// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "DrawDebugHelpers.h"
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

void UGWTraceBase::SingleLineTrace()
{
	FHitResult Impact;
	//TraceInterface->GetTargetData().Empty();
	const FVector ShootDir = GetPawnCameraAim();
	
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (GetStartLocationFromSocket() + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//TraceInterface->SetHitLocation(StartTrace, EndTrace, Impact.Actor.Get());
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	//	TraceInterface->SetHitLocation(StartTrace, EndTrace, Impact.Actor.Get());
	}
	TArray<FHitResult> TargetData;
	TargetData.Add(Impact);
	//TraceInterface->SetTargetData(TargetData);
	//GetOuterAGASAbility()->TargetData.Add(Impact);
}

void UGWTraceBase::SingleLineTraceSetHitLocation()
{
	const FVector ShootDir = GetPawnCameraAim();
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (GetStartLocationFromSocket() + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//TraceInterface->SetHitLocation(StartTrace, Impact.Location, Impact.Actor.Get());
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//TraceInterface->SetHitLocation(StartTrace, Impact.Location, Impact.Actor.Get());
	}
}

FVector UGWTraceBase::GetSingHitLocation()
{
	FHitResult Impact;
	const FVector ShootDir = GetPawnCameraAim();
	//
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (GetStartLocationFromSocket() + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//TraceInterface->SetHitLocation(StartTrace, EndTrace, Impact.Actor.Get());
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		//TraceInterface->SetHitLocation(StartTrace, EndTrace, Impact.Actor.Get());
	}
	
	return Impact.Location;
}
void UGWTraceBase::Initialize()
{
	UObject* out = GetOuter();
	//TraceInterface = Cast<IIGTTrace>(GetOuter());
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
	SingleLineTrace();
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

	//if (TraceInterface->GetPawn())
	//{
	//	FinalAim = TraceInterface->GetPawn()->GetBaseAimRotation().Vector();
	//}
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

FVector UGWTraceBase::GetStartLocationFromSocket()
{
	return FVector::ZeroVector; // TraceInterface->GetSocketLocation(StartLocation);
}

FVector UGWTraceBase::GetPawnCameraDamageStartLocation(const FVector& AimDir)
{
	FVector OutStartTrace = FVector::ZeroVector;
	//if (TraceInterface->GetPawn())
	//{
	//	FRotator UnusedRot;
	//	if (TraceInterface->GetPC())
	//	{
	//		TraceInterface->GetPC()->GetPlayerViewPoint(OutStartTrace, UnusedRot);
	//	}
	//	FVector ActorLocation = TraceInterface->GetPawn()->GetActorLocation();
	//	OutStartTrace = OutStartTrace + AimDir * ((ActorLocation - OutStartTrace) | AimDir);
	//	
	//	//OutStartTrace = OutStartTrace + AimDir * ((GetOuterAGASAbility()->PawnOwner->GetActorLocation() - OutStartTrace));
	//	return OutStartTrace;
	//}
	return OutStartTrace;
}

FHitResult UGWTraceBase::SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FHitResult Hit(ForceInit);
	UWorld* world = GetWorld();
	//if (!TraceInterface->GetPawn())
	//	return Hit;

	//static FName PowerTag = FName(TEXT("SingleLineTrace"));
	//FCollisionQueryParams TraceParams(PowerTag, false, TraceInterface->GetPawn());
	//TraceParams.bTraceComplex = false;
	//TraceParams.bTraceAsyncScene = false;
	//TraceParams.bReturnPhysicalMaterial = true;
	
	//if (bIgnoreSelf)
	//{
	//	TraceParams.AddIgnoredActor(TraceInterface->GetPawn());
	//}

	//bool traceResult = GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, TraceParams, CollisionObjectParams);
	//if (bDrawDebug)
	//{
	//	if (traceResult && Hit.bBlockingHit)
	//	{
	//		::DrawDebugLine(world, StartTrace, Hit.ImpactPoint, FColor::Red, false, 2);
	//		::DrawDebugLine(world, Hit.ImpactPoint, EndTrace, FColor::Green, false, 2);
	//		::DrawDebugPoint(world, Hit.ImpactPoint, 7, FColor::Red, false, 2);
	//	}
	//	else
	//	{
	//		::DrawDebugPoint(world, Hit.ImpactPoint, 15, FColor::Red, false, 2);
	//	}
	//}
	return Hit;
}


FVector UGWTraceBase::GetHelperScale()
{
	return FVector::ZeroVector;
}