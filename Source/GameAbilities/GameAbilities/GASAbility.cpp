// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "States/GASAbilityState.h"
#include "States/GASAbilityStatePreparation.h"

#include "Actions/GASAbilityAction.h"

#include "GTTraceBase.h"

#include "IGASAbilities.h"

#include "GASAbility.h"

AGASAbility::AGASAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	SetReplicates(true);

	PreparationState = ObjectInitializer.CreateDefaultSubobject<UGASAbilityStatePreparation>(this, TEXT("PreparationState"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
}

void AGASAbility::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CurrentState)
		CurrentState->Tick(DeltaSeconds);
	/*
		Tick current targeting action. For most abilities it won't do anything, for most part.
	*/
	if (Targeting)
		Targeting->Tick(DeltaSeconds);
}

void AGASAbility::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = ActiveState;
	//if (TargetingAction)
	//	TargetingAction->Initialize();
	if (Targeting)
		Targeting->Initialize();
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
	ActivateAbility();
}

void AGASAbility::InputReleased()
{
	DeactivateAbility();
}

void AGASAbility::ActivateAbility()
{
	if (Role < ROLE_Authority)
	{
		ServerActivateAbility();
	}
	else
	{
		CurrentState->BeginActionSequence();
	}
}

void AGASAbility::ExecuteAbility()
{
	//TargetingAction->Execute();
	Targeting->Execute();
	OnAbilityActivated();
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

void AGASAbility::RunPreparationActions()
{
	if (Targeting)
		Targeting->PreExecute();
}


FVector AGASAbility::GetSocketLocation(FName SocketNameIn)
{
	FVector SocketLocation = FVector::ZeroVector;
	IIGASAbilities* abilityInt = Cast<IIGASAbilities>(PawnOwner);
	if (abilityInt)
	{
		SocketLocation = abilityInt->GetSocketLocation(SocketNameIn);
	}
	return SocketLocation;
}
void AGASAbility::SetTargetData(const TArray<FHitResult>& DataIn)
{ 
	TargetData = DataIn; 
};