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
	CurrentCastTime = 0;
	CurrentCooldownTime = 0;
	CastTimeModifier = 1;

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
	PrimaryActorTick.RegisterTickFunction(GetLevel());
	CurrentState = ActiveState;
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

void AGASAbility::OnRep_PawnOwner()
{
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
		if (CheckIfCanUseAbility())
		{
			ServerActivateAbility();
			//simulate on client.
			//so we can predictivly start spawning effect cues.
			CurrentState->BeginActionSequence();
		}
	}
	else
	{
		if (CheckIfCanUseAbility()) //double check in case client is cheating..
		{
			CurrentState->BeginActionSequence();
		}
	}
}

void AGASAbility::ExecuteAbility()
{
	//TargetingAction->Execute();
	if (Role < ROLE_Authority)
	{
		//simulate on client
		//so we can get targeting data on client, and start spawning cue effects predictively.
		Targeting->Execute();
		ServerExecuteAbility();
	}
	else
	{
		Targeting->Execute();
		//actuall ability logic triggered by this event, run only on server.
		OnAbilityActivated();
	}
}

void AGASAbility::ExecuteAbilityPeriod()
{
	if (Role < ROLE_Authority)
	{
		OnAbilityHit(AbilityHitInfo.Origin, AbilityHitInfo.HitLocation, AbilityHitInfo.HitActor);
		ServerExecuteAbilityPeriod();
	}
	else
	{
		//update hitinfo, even though we don't get any new location information ?
		//probabaly could add bool to controll it, 
		AbilityHitInfo.Counter++;
		OnAbilityActivated();
	}
}
void AGASAbility::ServerExecuteAbilityPeriod_Implementation()
{
	ExecuteAbilityPeriod();
}
bool AGASAbility::ServerExecuteAbilityPeriod_Validate()
{
	return true;
}

void AGASAbility::ExecuteTargeting()
{
	Targeting->Execute();
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
		CurrentState->EndActionSequence();
		ServerDeactivateAbility();
	}
	else
	{
		CurrentState->EndActionSequence();
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
	//should run on client as well.
	//for debug drawing and possibly simulate hit indicators etc.
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
	if (Role < ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnAbilityCastStart.Broadcast();
		OnAbilityCastStarted();
	}
}
void AGASAbility::AbilityCastEnd()
{
	AbilityCastEnded++;
	//if (GetNetMode() == ENetMode::NM_DedicatedServer)
	//	ClientAbilityCastingEnd();

	//call only on server so, we can begin cleanup for server side spawned objects
	//which are bound to this delegate.
	if (Role == ROLE_Authority || GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		OnAbilityCastEnd.Broadcast();
	}

	if (Role < ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnAbilityCastEnd.Broadcast();
		OnAbilityCastEnded();
	}
}

void AGASAbility::AbilityPreparationStart()
{
	PreparationStarted++;
	if (Role < ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnAbilityPreparationStart.Broadcast();
		OnAbilityPreperationStarted();
	}
}
void AGASAbility::AbilityPreparationEnd()
{
	PreparationEnd++;
	//make sure that client owning this ability
	//will properly end current state.
	//if (GetNetMode() == ENetMode::NM_DedicatedServer)
	//	OnAbilityPreperationEnd();

	if (Role < ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnAbilityPreparationEnd.Broadcast();
		OnAbilityPreperationEnded();
	}
}

bool AGASAbility::CheckIfCanUseAbility()
{
	return true;
}

void AGASAbility::SetHitLocation(const FVector& Origin, const FVector& HitLocation, AActor* HitActor)
{
	SetAbilityHitInfo(Origin, HitLocation, HitActor);
}

void AGASAbility::SetAbilityHitInfo(const FVector& Origin, const FVector& HitLocation, AActor* HitTarget)
{
	AbilityHitInfo.Counter++;
	AbilityHitInfo.Origin = Origin;
	AbilityHitInfo.HitLocation = HitLocation;
	AbilityHitInfo.HitActor = HitTarget;
	if (Role < ROLE_Authority || GetNetMode() == ENetMode::NM_Standalone)
		OnAbilityHit(AbilityHitInfo.Origin, AbilityHitInfo.HitLocation, AbilityHitInfo.HitActor);
}
void AGASAbility::ClientAbilityCastingEnd_Implementation()
{
	OnAbilityCastEnded();
}
void AGASAbility::ClientAbilityPreparationEnd_Implementation()
{
	OnAbilityPreperationEnded();
}
void AGASAbility::OnAbilityCastStarted_Implementation()
{
}
void AGASAbility::OnAbilityCastEnded_Implementation()
{
}

void AGASAbility::OnAbilityPreperationStarted_Implementation()
{
}
void AGASAbility::OnAbilityPreperationEnded_Implementation()
{
}

void AGASAbility::OnAbilityHit_Implementation(const FVector& Origin, const FVector& HitLocation, AActor* TargetActor)
{
}

void AGASAbility::OnRep_CastStarted()
{
	OnAbilityCastStart.Broadcast();
	OnAbilityCastStarted();
}
void AGASAbility::OnRep_CastEnded()
{
	OnAbilityCastEnd.Broadcast();
	OnAbilityCastEnded();
}

void AGASAbility::OnRep_PreparationStarted()
{
	OnAbilityPreparationStart.Broadcast();
	OnAbilityPreperationStarted();
}
void AGASAbility::OnRep_PreparationEnded()
{
	OnAbilityPreparationEnd.Broadcast();
	OnAbilityPreperationEnded();
}

void AGASAbility::OnRep_AbilityHitInfo()
{
	OnAbilityHit(AbilityHitInfo.Origin, AbilityHitInfo.HitLocation, AbilityHitInfo.HitActor);
}

void AGASAbility::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGASAbility, CueReplication, COND_SkipOwner);
	DOREPLIFETIME(AGASAbility, PawnOwner);

	DOREPLIFETIME(AGASAbility, TargetData);
	//locally we will play those effects immidietly on client.
	//regradless if there is confirmd success from server or not.
	//if there is no success on server, we will just override them.
	DOREPLIFETIME_CONDITION(AGASAbility, AbilityCastStarted, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AGASAbility, AbilityCastEnded, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AGASAbility, PreparationStarted, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AGASAbility, PreparationEnd, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AGASAbility, AbilityHitInfo, COND_SkipOwner);
}