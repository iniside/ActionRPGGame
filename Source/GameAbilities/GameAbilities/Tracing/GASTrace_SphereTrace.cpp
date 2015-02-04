// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GTTraceDisplayHelper.h"
#include "IGTTrace.h"
#include "GASTrace_SphereTrace.h"

UGASTrace_SphereTrace::UGASTrace_SphereTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
void UGASTrace_SphereTrace::Tick(float DeltaTime)
{
	if (TargetingHelper)
	{
		if (GetOuterUGASAbility()->GetNetMode() == ENetMode::NM_Standalone
			|| GetOuterUGASAbility()->GetNetMode() == ENetMode::NM_Client)
			TargetingHelper->SetActorLocation(GetSingHitLocation());
	}
}
void UGASTrace_SphereTrace::Initialize()
{
	Super::Initialize();

	
	for (auto Iter = SphereObjectToTrace.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = (*Iter);
		if (FCollisionObjectQueryParams::IsValidObjectQuery(Channel))
		{
			SphereCollisionObjectParams.RemoveObjectTypesToQuery(Channel);
			SphereCollisionObjectParams.AddObjectTypesToQuery(Channel);
		}
	}
}
void UGASTrace_SphereTrace::PreExecute()
{
	if (!TargetingHelperClass)
		return;

	if (TargetingHelper)
	{
		TargetingHelper->Destroy();
	}
	TargetingHelper = GetWorld()->SpawnActor<AGTTraceDisplayHelper>(TargetingHelperClass);
	TargetingHelper->TraceAction = this;
	TargetingHelper->AreaSize = TraceArea;
	TargetingHelper->OnPostInitialize();
}
void UGASTrace_SphereTrace::Execute()
{
	SphereTrace();
	PostExecute();
}
void UGASTrace_SphereTrace::PostExecute()
{
	if (TargetingHelper)
	{
		TargetingHelper->TraceAction = nullptr;
		TargetingHelper->Destroy();
	}
}

void UGASTrace_SphereTrace::SphereTrace()
{
	GetOuterUGASAbility()->TargetData.Empty();

	static const FName AbilitySphereTraceMultiName(TEXT("AbilitySphereTraceMulti"));
	FCollisionQueryParams Params(AbilitySphereTraceMultiName, false, TraceInterface->GetGamePawn());
	Params.bTraceComplex = false;
	Params.bTraceAsyncScene = false;
	Params.bReturnPhysicalMaterial = true;

	Params.AddIgnoredActor(TraceInterface->GetGamePawn());
	Params.bFindInitialOverlaps = true;

	FVector StartLocation = GetSingHitLocation();
	TArray<FOverlapResult> Overlaps;
	bool bHit;

	//don't like it. But sweep is just useless in this scenario.
	bHit = GetWorld()->OverlapMulti(Overlaps, StartLocation, FQuat::Identity, FCollisionShape::MakeSphere(TraceArea.X), Params, SphereCollisionObjectParams);
	TArray<FHitResult> hitDebugs;
	for (const FOverlapResult overlap : Overlaps)
	{
		FHitResult hit;

		hit.Actor = overlap.Actor.Get();
		hit.bBlockingHit = overlap.bBlockingHit;
		if (hit.Actor.IsValid())
			hit.Location = hit.Actor->GetActorLocation();
		hit.Location.Z += 50;
		hit.ImpactPoint = hit.Location;
		GetOuterUGASAbility()->HitTarget(overlap.Actor.Get(), hit.Location, hit);
		hitDebugs.Add(hit);
	}

	if (bDrawDebug)
	{
		if (bHit && hitDebugs.Last().bBlockingHit)
		{
			FVector const BlockingHitPoint = hitDebugs.Last().Location;
			DrawDebugSphere(GetWorld(), StartLocation, TraceArea.X, 32, FColor::Red, false, 2, 0);
		}
		else
		{
			DrawDebugSphere(GetWorld(), StartLocation, TraceArea.X, 32, FColor::Blue, false, 2, 0);
		}

		int32 HitCount = hitDebugs.Num();
		// draw hits
		for (int32 HitIdx = 0; HitIdx < HitCount; ++HitIdx)
		{
			FHitResult const& Hit = hitDebugs[HitIdx];
			::DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 30, (Hit.bBlockingHit ? FColor::Red : FColor::Green), false, 3, 0);
		}
	}
}