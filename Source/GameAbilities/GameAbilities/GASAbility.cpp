// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "States/GASAbilityState.h"
#include "GASAbility.h"

AGASAbility::AGASAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	SetReplicates(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
}

void AGASAbility::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CurrentState->Tick(DeltaSeconds);
}

void AGASAbility::GotoState(class UGASAbilityState* NextState)
{
	if (NextState == NULL || !NextState->IsIn(this))
	{

	}
	else if (CurrentState != NextState)
	{
		UGASAbilityState* PrevState = CurrentState;
		if (CurrentState != NULL)
		{
			CurrentState->EndState();
		}
		if (CurrentState == PrevState)
		{
			CurrentState = NextState;
			CurrentState->BeginState(PrevState);
		}
	}
}

void AGASAbility::InputActivate()
{

}

void AGASAbility::ActivateAbility()
{
	OnAbilityActivated();
}