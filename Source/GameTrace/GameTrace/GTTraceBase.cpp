// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameTrace.h"
#include "IGIPawn.h"
#include "IGISkeletalMesh.h"
#include "DrawDebugHelpers.h"
#include "GTTraceBase.h"

UGTTraceBase::UGTTraceBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
UWorld* UGTTraceBase::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UGTTraceBase::BeginDestroy()
{
	TraceInterface = nullptr;
	Super::BeginDestroy();
}
void UGTTraceBase::Tick(float DeltaSecondsIn)
{

}

FHitResult UGTTraceBase::GetSingleHitResult()
{
	FHitResult Impact;
	const FVector ShootDir = GetPawnCameraAim();
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (GetStartLocationFromSocket() + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		Impact = SingleLineRangedTrace(StartTrace, EndTrace);
	}
	return Impact;
}

FVector UGTTraceBase::GetSingHitLocation()
{
	
	const FVector ShootDir = GetPawnCameraAim();
	const FVector FinalAim = GetPawnCameraDamageStartLocation(ShootDir);
	if (bTraceFromSocket)
	{
		const FVector StartTrace = GetStartLocationFromSocket();
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		if (Impact.bBlockingHit)
			return Impact.Location;
		else
			return EndTrace;
	}
	else
	{
		const FVector StartTrace = GetPawnCameraDamageStartLocation(ShootDir);
		const FVector EndTrace = (StartTrace + ShootDir * Range);
		const FHitResult Impact = SingleLineRangedTrace(StartTrace, EndTrace);
		if (Impact.bBlockingHit)
			return Impact.Location;
		else
			return EndTrace;
	}
	return FVector::ZeroVector;
}

void UGTTraceBase::Initialize()
{
	UObject* out = GetOuter();
	TraceInterface = Cast<IIGIPawn>(GetOuter());
	SkeletalMeshInt = Cast<IIGISkeletalMesh>(GetOuter());
	for (auto Iter = ObjectsToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			CollisionObjectParams.AddObjectTypesToQuery(Channel);
		}
	}
}

void UGTTraceBase::PreExecute()
{
	//display something. Like spawn targeting actor circle. Or sphere. Or whatever.
}
void UGTTraceBase::Execute()
{
}

void UGTTraceBase::PostExecute()
{
}

void UGTTraceBase::ModifyRangeByPrecentage(float PrcentageIn)
{

}

FVector UGTTraceBase::GetPawnCameraAim()
{
	FVector FinalAim = FVector::ZeroVector;
	FRotator FinalRot = FRotator(0, 0, 0);
	if (TraceInterface->GetGamePawn())
	{
		FinalAim = TraceInterface->GetGamePawn()->GetBaseAimRotation().Vector();
		//TraceInterface->GetPawn()->GetActorEyesViewPoint(FinalAim, FinalRot);
	}
	return FinalAim;// .Vector();
}

FVector UGTTraceBase::GetPCCameraAim()
{
	FVector OutStartTrace = FVector::ZeroVector;
	FRotator AimDir;
	if (TraceInterface->GetGamePlayerController())
	{
		TraceInterface->GetGamePlayerController()->GetPlayerViewPoint(OutStartTrace, AimDir);
	}
	return AimDir.Vector();
}

FVector UGTTraceBase::GetStartLocationFromSocket()
{
	return SkeletalMeshInt->GetSocketLocation(StartLocation);
}

FVector UGTTraceBase::GetPawnCameraDamageStartLocation(const FVector& AimDir)
{
	FVector OutStartTrace = FVector::ZeroVector; //change it to socket location
	if (TraceInterface->GetGamePawn())
	{
		FRotator UnusedRot;
		if (TraceInterface->GetGamePlayerController())
		{
			TraceInterface->GetGamePlayerController()->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		}
		FVector ActorLocation = TraceInterface->GetGamePawn()->GetActorLocation();
		OutStartTrace = OutStartTrace + AimDir * ((ActorLocation - OutStartTrace) | AimDir);
		return OutStartTrace;
	}
	return OutStartTrace;
}
FVector UGTTraceBase::GetPawnSocketDamageStartLocation(const FVector& AimDir)
{
	FVector SocketStartLocation = GetStartLocationFromSocket();
	if (TraceInterface->GetGamePawn())
	{
		FVector ActorLocation = TraceInterface->GetGamePawn()->GetActorLocation();
		SocketStartLocation = SocketStartLocation + AimDir * ((ActorLocation - SocketStartLocation) | AimDir);
		return SocketStartLocation;
	}
	return SocketStartLocation;
}
FHitResult UGTTraceBase::SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FHitResult Hit(ForceInit);
	UWorld* world = GetWorld();
	if (!TraceInterface->GetGamePawn())
		return Hit;

	static FName PowerTag = FName(TEXT("SingleLineTrace"));
	FCollisionQueryParams TraceParams(PowerTag, false, TraceInterface->GetGamePawn());
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = true;
	
	if (bIgnoreSelf)
	{
		TraceParams.AddIgnoredActor(TraceInterface->GetGamePawn());
	}

	/*bool traceResult = GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, TraceParams, CollisionObjectParams);
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
	}*/
	return Hit;
}