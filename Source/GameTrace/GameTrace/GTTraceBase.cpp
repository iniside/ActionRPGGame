// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameTrace.h"
#include "IGTTrace.h"
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

void UGTTraceBase::SingleLineTrace()
{
	FHitResult Impact;
	TraceInterface->GetTargetData().Empty();
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
	TArray<FHitResult> TargetData;
	TargetData.Add(Impact);
	TraceInterface->SetTargetData(TargetData);
	//GetOuterAGASAbility()->TargetData.Add(Impact);
}
FVector UGTTraceBase::GetSingHitLocation()
{
	FHitResult Impact;
	const FVector ShootDir = GetPawnCameraAim();
	//
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

	return Impact.Location;
}
void UGTTraceBase::Initialize()
{
	UObject* out = GetOuter();
	TraceInterface = Cast<IIGTTrace>(GetOuter());
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
	SingleLineTrace();
}

void UGTTraceBase::PostExecute()
{

}

void UGTTraceBase::ModifyExtendsByPrecentage(float PrcentageIn)
{

}
void UGTTraceBase::BP_ModiyExtendsByPrcentage(float PrecentageIn)
{
	ModifyExtendsByPrecentage(PrecentageIn);
}
void UGTTraceBase::ModifyRangeByPrecentage(float PrcentageIn)
{

}

void UGTTraceBase::SetTraceRange(float PrcentageIn)
{

}

FVector UGTTraceBase::GetPawnCameraAim()
{
	FVector FinalAim = FVector::ZeroVector;

	if (TraceInterface->GetPawn())
	{
		FinalAim = TraceInterface->GetPawn()->GetBaseAimRotation().Vector();
	}
	return FinalAim;
}

FVector UGTTraceBase::GetPCCameraAim()
{
	FVector OutStartTrace = FVector::ZeroVector;
	FRotator AimDir;
	if (TraceInterface->GetPC())
	{
		TraceInterface->GetPC()->GetPlayerViewPoint(OutStartTrace, AimDir);
	}
	return AimDir.Vector();
}

FVector UGTTraceBase::GetStartLocationFromSocket()
{
	return TraceInterface->GetSocketLocation(StartLocation);
}

FVector UGTTraceBase::GetPawnCameraDamageStartLocation(const FVector& AimDir)
{
	FVector OutStartTrace = FVector::ZeroVector;
	if (TraceInterface->GetPawn())
	{
		FRotator UnusedRot;
		if (TraceInterface->GetPC())
		{
			TraceInterface->GetPC()->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		}
		FVector ActorLocation = TraceInterface->GetPawn()->GetActorLocation();
		OutStartTrace = OutStartTrace + AimDir * ((ActorLocation - OutStartTrace) | AimDir);
		
		//OutStartTrace = OutStartTrace + AimDir * ((GetOuterAGASAbility()->PawnOwner->GetActorLocation() - OutStartTrace));
		return OutStartTrace;
	}
	return OutStartTrace;
}

FHitResult UGTTraceBase::SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FHitResult Hit(ForceInit);
	UWorld* world = GetWorld();
	if (!TraceInterface->GetPawn())
		return Hit;

	static FName PowerTag = FName(TEXT("SingleLineTrace"));
	FCollisionQueryParams TraceParams(PowerTag, false, TraceInterface->GetPawn());
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = true;
	
	if (bIgnoreSelf)
	{
		TraceParams.AddIgnoredActor(TraceInterface->GetPawn());
	}

	bool traceResult = GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, TraceParams, CollisionObjectParams);
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
