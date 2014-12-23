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

void AGASAbility::InputPressed()
{
	ActivateAbility(); //temporary this never should be called directly.
	//will change one state machine and replication is fully operational.
}

void AGASAbility::InputReleased()
{
	DeactivateAbility(); //temporary his never should call it directly.
}

void AGASAbility::ActivateAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerActivateAbility();
	}
	else
	{
		OnAbilityActivated();
	}
}
void AGASAbility::ServerActivateAbility_Implementation()
{
	ActivateAbility();
}
bool AGASAbility::ServerActivateAbility_Validate()
{
	return true;
}

void AGASAbility::DeactivateAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerDeactivateAbility();
	}
	else
	{
		OnAbilityDeactivated();
	}
}
void AGASAbility::ServerDeactivateAbility_Implementation()
{
	DeactivateAbility();
}
bool AGASAbility::ServerDeactivateAbility_Validate()
{
	return true;
}