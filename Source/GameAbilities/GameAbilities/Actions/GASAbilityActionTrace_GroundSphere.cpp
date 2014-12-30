// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "DrawDebugHelpers.h"
#include "GASAbilityActionTrace_GroundSphere.h"

UGASAbilityActionTrace_GroundSphere::UGASAbilityActionTrace_GroundSphere(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}

void UGASAbilityActionTrace_GroundSphere::Tick(float DeltaSecondsIn)
{
	if (DisplayHelper)
	{
		if (GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Standalone
			|| GetOuterAGASAbility()->GetNetMode() == ENetMode::NM_Client)
			DisplayHelper->SetActorLocation(SingleLineTrace());
	}
}

void UGASAbilityActionTrace_GroundSphere::PreExecute()
{
	if (DisplayHelperActor)
	{
		DisplayHelper = GetWorld()->SpawnActor<AActor>(DisplayHelperActor);
	}
}
void UGASAbilityActionTrace_GroundSphere::Execute()
{
	PostExecute();
}

void UGASAbilityActionTrace_GroundSphere::PostExecute()
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