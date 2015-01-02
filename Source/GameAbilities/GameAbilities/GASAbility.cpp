// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "States/GASAbilityState.h"
#include "States/GASAbilityStatePreparation.h"

#include "Actions/GASAbilityAction.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GTTraceBase.h"

#include "IGASAbilities.h"
#include "IGTSocket.h"

#include "GASAbility.h"

AGASAbility::AGASAbility(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	SetReplicates(true);
	iSocket = nullptr;
	PreparationState = ObjectInitializer.CreateDefaultSubobject<UGASAbilityStatePreparation>(this, TEXT("PreparationState"));

	//Targeting = ObjectInitializer.CreateDefaultSubobject<UGTTraceBase>(this, TEXT("TraceBase"));

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
void AGASAbility::BeginDestroy()
{
	iSocket = nullptr;
	Super::BeginDestroy();
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
void AGASAbility::SetPawnOwner(APawn* PawnOwnerIn)
{
	PawnOwner = PawnOwnerIn;
	switch (TraceFrom)
	{
		case EGASTraceAbility::Pawn:
			iSocket = Cast<IIGTSocket>(PawnOwner);
			break;
		case EGASTraceAbility::Avatar:
			iSocket = Cast<IIGTSocket>(AvatarActor);
			break;
		case EGASTraceAbility::Invalid:
			break;
		default:
			break;
	}
	
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
	if (Role < ROLE_Authority)
	{
		ServerExecuteAbility();
	}
	else
	{
		Targeting->Execute();
		OnAbilityActivated();
	}
}
void AGASAbility::ServerExecuteAbility_Implementation()
{
	ExecuteAbility();
}
bool AGASAbility::ServerExecuteAbility_Validate()
{
	return true;
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
	if (iSocket)
	{
		SocketLocation = iSocket->GetSocketLocation(SocketNameIn);
	}
	return SocketLocation;
}
void AGASAbility::SetTargetData(const TArray<FHitResult>& DataIn)
{ 
	TargetData = DataIn;
	CueReplication.Targets.Empty();
	for (const FHitResult& hit : TargetData)
	{
		CueReplication.Targets.Add(hit.Actor.Get());
	}
};
void AGASAbility::OnRep_CueRep()
{
	for (AActor* actor : CueReplication.Targets)
	{
		//spawn effects.
		//it might be problematic for abilities which are channeled
		//means they happen over time, since well, we don't have any indicator
		//if ability casting is finished or just started kewl.
	}
}

void AGASAbility::AbilityCastStart()
{
	AbilityCastStarted++;
}
void AGASAbility::AbilityCastEnd()
{
	AbilityCastEnded++;
}
void AGASAbility::AbilityPreparationStart()
{
	PreparationStarted++;
}
void AGASAbility::AbilityPreparationEnd()
{
	PreparationEnd++;
}


bool AGASAbility::OnAbilityCastStarted_Implementation()
{
	return false;
}
bool AGASAbility::OnAbilityCastEnd_Implementation()
{
	return false;
}

bool AGASAbility::OnAbilityPreperationStarted_Implementation()
{
	return false;
}
bool AGASAbility::OnAbilityPreperationEnd_Implementation()
{
	return false;
}

void AGASAbility::OnRep_CastStarted()
{
	OnAbilityCastStarted();
}
void AGASAbility::OnRep_CastEnded()
{
	OnAbilityCastEnd();
}

void AGASAbility::OnRep_PreparationStarted()
{
	OnAbilityPreperationStarted();
}
void AGASAbility::OnRep_PreparationEnded()
{
	OnAbilityPreperationEnd();
}

void AGASAbility::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGASAbility, CueReplication, COND_SkipOwner);
	DOREPLIFETIME(AGASAbility, PawnOwner);
	//locally we will play those effects immidietly on client.
	//regradless if there is confirmd success from server or not.
	//if there is no success on server, we will just override them.
	DOREPLIFETIME(AGASAbility, AbilityCastStarted);
	DOREPLIFETIME_CONDITION(AGASAbility, AbilityCastEnded, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AGASAbility, PreparationStarted, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AGASAbility, PreparationEnd, COND_SkipOwner);
}