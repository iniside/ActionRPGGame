// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameTrace.h"
#include "DrawDebugHelpers.h"

#include "GTTraceDisplayHelper.h"

#include "IGTTrace.h"
#include "GTTrace_GroundSphere.h"

UGTTrace_GroundSphere::UGTTrace_GroundSphere(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}

void UGTTrace_GroundSphere::Tick(float DeltaSecondsIn)
{
	//if (DisplayHelper)
	//{
	//	if (GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Standalone
	//		|| GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Client)
	//		DisplayHelper->SetActorLocation(GetSingHitLocation());
	//}
}
void UGTTrace_GroundSphere::Initialize()
{
	//calling super would be enough.

	for (auto Iter = SphereObjectToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			SphereObjectParams.AddObjectTypesToQuery(Channel);
		}
	}
	for (auto Iter = ObjectsToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			CollisionObjectParams.AddObjectTypesToQuery(Channel);
		}
	}
}
void UGTTrace_GroundSphere::PreExecute()
{
	if (DisplayHelperActor)
	{
		DisplayHelper = GetWorld()->SpawnActor<AGTTraceDisplayHelper>(DisplayHelperActor);
		DisplayHelper->TraceAction = this;
	}
}
void UGTTrace_GroundSphere::Execute()
{
	SphereTrace();
	PostExecute();
}

void UGTTrace_GroundSphere::PostExecute()
{
	if (DisplayHelper)
	{
		DisplayHelper->SetActorHiddenInGame(true);
		DisplayHelper->Destroy();
		/*
			We could cache actor, and don't destroy it to avoid spawning it every time
			owning ability is called.

			We could destroy it when ability is no longer active (ie removed from hot bar).
			Or at any other time, which could be decided by whoever extend this class.

			But this solution is simplest, doesn't require any magic, and it's run only for client/player,
			so nothing should burn ;).
		*/
	}
}
void UGTTrace_GroundSphere::SphereTrace()
{
	TraceInterface->GetTargetData().Empty();

	static const FName SphereTraceMultiName(TEXT("SphereTraceMulti"));
	FCollisionQueryParams Params(SphereTraceMultiName, false, TraceInterface->GetPawn());
	Params.bTraceComplex = false;
	Params.bTraceAsyncScene = false;
	Params.bReturnPhysicalMaterial = true;

	Params.AddIgnoredActor(TraceInterface->GetPawn());
	FVector StartLocation = GetSingHitLocation();
	TArray<FOverlapResult> Overlaps;
	bool bHit;

	//don't like it. But sweep is just useless in this scenario.
	bHit = GetWorld()->OverlapMulti(Overlaps, StartLocation, FQuat::Identity, FCollisionShape::MakeSphere(Radius), Params, SphereObjectParams);

	for (const FOverlapResult overlap : Overlaps)
	{
		FHitResult hit;

		hit.Actor = overlap.Actor.Get();
		hit.bBlockingHit = overlap.bBlockingHit;
		hit.Location = hit.Actor->GetActorLocation();
		hit.Location.Z += 50;
		hit.ImpactPoint = hit.Location;
		TraceInterface->GetTargetData().Add(hit);
	}

	if (bDrawDebug)
	{
		if (bHit && TraceInterface->GetTargetData().Last().bBlockingHit)
		{
			FVector const BlockingHitPoint = TraceInterface->GetTargetData().Last().Location;
			DrawDebugSphere(GetWorld(), StartLocation, Radius, 32, FColor::Red, false, 2, 0);
		}
		else
		{
			DrawDebugSphere(GetWorld(), StartLocation, Radius, 32, FColor::Blue, false, 2, 0);
		}

		int32 HitCount = TraceInterface->GetTargetData().Num();
		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitCount; ++HitIdx)
		{
			FHitResult const& Hit = TraceInterface->GetTargetData()[HitIdx];
			::DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 30, (Hit.bBlockingHit ? FColor::Red : FColor::Green), false, 3,0);
		}
	}
}
