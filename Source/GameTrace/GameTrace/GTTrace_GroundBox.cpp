// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameTrace.h"
#include "DrawDebugHelpers.h"
#include "IGTTrace.h"
#include "GTTrace_GroundBox.h"

UGTTrace_GroundBox::UGTTrace_GroundBox(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}

void UGTTrace_GroundBox::Tick(float DeltaSecondsIn)
{
	//if (DisplayHelper)
	//{
	//	if (GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Standalone
	//		|| GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Client)
	//		DisplayHelper->SetActorLocation(GetSingHitLocation());
	//}
	if (bDrawDebug)
	{
		if ((TraceInterface && TraceInterface->GetPawn())
			&& (TraceInterface->GetPawn()->Role < ROLE_Authority
			|| TraceInterface->GetPawn()->GetNetMode() == ENetMode::NM_Standalone))
		{
			FRotator aimRot = TraceInterface->GetPawn()->GetActorRotation();
			FVector Location = GetSingHitLocation();
			Location.X += BoxSize.X;
			Location.Z += BoxSize.Z;
			//aimRot.Roll = 0;
			aimRot.Pitch = 0;
			DrawDebugBox(GetWorld(), Location, BoxSize, FQuat(aimRot), FColor::Red, true, 0.1f, 0);
		}
	}
}

void UGTTrace_GroundBox::Initialize()
{
	for (auto Iter = BoxObjectsToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			BoxObjectParams.AddObjectTypesToQuery(Channel);
		}
	}

	Super::Initialize();
}
void UGTTrace_GroundBox::PreExecute()
{
	if (DisplayHelperActor)
	{
		DisplayHelper = GetWorld()->SpawnActor<AActor>(DisplayHelperActor);
	}
}
void UGTTrace_GroundBox::Execute()
{
	TraceBox();
	PostExecute();
}

void UGTTrace_GroundBox::PostExecute()
{

}

void UGTTrace_GroundBox::TraceBox()
{
	TraceInterface->GetTargetData().Empty();

	static const FName BoxTraceMultiTraceMultiName(TEXT("BoxTraceMulti"));
	FCollisionQueryParams Params(BoxTraceMultiTraceMultiName, false, TraceInterface->GetPawn());
	Params.bTraceComplex = false;
	Params.bTraceAsyncScene = false;
	Params.bReturnPhysicalMaterial = true;

	Params.AddIgnoredActor(TraceInterface->GetPawn());
	FRotator aimRot = TraceInterface->GetPawn()->GetBaseAimRotation();
	aimRot.Roll = 0;
	aimRot.Pitch = 0;
	FVector StartLocation = GetSingHitLocation();
	FVector HitLocation = StartLocation;
	StartLocation.X += BoxSize.X;
	StartLocation.Z += BoxSize.Z;
	FVector EndLocation = StartLocation + .1;
	TArray<FOverlapResult> Overlaps;
	bool bHit;
	FVector Origin = FVector::ZeroVector;
	if (bTraceFromSocket)
	{
		Origin = GetStartLocationFromSocket();
	}
	else
	{
		Origin = GetPawnCameraDamageStartLocation(GetPawnCameraAim());
	}

	TraceInterface->SetHitLocation(Origin, HitLocation, nullptr);
	//don't like it. But sweep is just useless in this scenario.
	bHit = GetWorld()->OverlapMulti(Overlaps, StartLocation, FQuat(aimRot), FCollisionShape::MakeBox(BoxSize), Params, BoxObjectParams);

	for (const FOverlapResult overlap : Overlaps)
	{
		FHitResult hit;

		hit.Actor = overlap.Actor.Get();
		hit.bBlockingHit = overlap.bBlockingHit;
		hit.Location = hit.Actor->GetActorLocation();
		hit.Location.Z += 30;
		hit.ImpactPoint = hit.Location;
		TraceInterface->GetTargetData().Add(hit);
	}

	if (bDrawDebug)
	{
		if (bHit && TraceInterface->GetTargetData().Last().bBlockingHit)
		{
			DrawDebugBox(GetWorld(), StartLocation, BoxSize, FQuat(aimRot), FColor::Red, true, 3.f, 0);
		}
		else
		{
			DrawDebugBox(GetWorld(), StartLocation, BoxSize, FQuat(aimRot), FColor::Blue, true, 3.f, 0);
		}

		int32 HitCount = TraceInterface->GetTargetData().Num();
		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitCount; ++HitIdx)
		{
			FHitResult const& Hit = TraceInterface->GetTargetData()[HitIdx];
			::DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 30, (Hit.bBlockingHit ? FColor::Red : FColor::Green), false, 3, 0);
		}
	}
}