// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "IGISkeletalMesh.h"
#include "GASTrace.h"

UGASTrace::UGASTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}
void UGASTrace::Tick(float DeltaSecondsIn)
{

}
void UGASTrace::SingleLineTraceSetHitLocation()
{
	//const FHitResult hit = GetSingleHitResult();
	//const FVector Origin = SkeletalMeshInt->GetSocketLocation(StartLocation);
	//GetOuterUGASAbility()->SetHitLocation(Origin, hit.ImpactPoint);
}