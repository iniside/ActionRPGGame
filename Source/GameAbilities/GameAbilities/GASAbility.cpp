// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbilitiesComponent.h"
#include "States/GASAbilityState.h"
#include "Tracing/GASTrace.h"

#include "Net/UnrealNetwork.h"

#include "GASAbility.h"

UGASAbility::UGASAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicate = true;
	bIsNameStable = false;
	bShouldTick = false;
	bIsOnCooldown = false;

	bUpdateHitLocationEveryFrame = false;
}
void UGASAbility::Initialize()
{
	CurrentState = ActiveState;
	GISkeletalMesh = Cast<IIGISkeletalMesh>(POwner);
	Targeting->Initialize();
	Targeting->SetTraceRange(Range);
	Targeting->SetTraceArea(AbilityArea);
	bIsInitialized = true;
}
void UGASAbility::InputPressed()
{
	PrepareAbility();
}

void UGASAbility::InputReleased()
{
	if (CurrentState)
		CurrentState->EndActionSequence();
}
void UGASAbility::InputCancel()
{

}

void UGASAbility::CancelAbility()
{

}
void UGASAbility::PrepareAbility()
{
	//it will go to PreparationState
	//if thee PreaparationState is set to NoPrep it will, go from here straight to CommitAbility();
	CurrentState->BeginActionSequence();
}
void UGASAbility::RunPreparationActions()
{
	OnAbilityPrepare();
	if (Targeting)
	{
		Targeting->SetTraceRange(Range);
		Targeting->SetTraceArea(AbilityArea);
		Targeting->PreExecute();
	}
}
bool UGASAbility::CommitAbility()
{
	bool bAbilityCommited = false;
	if (!CanCommitAbility())
		return bAbilityCommited;
	CommitResources();

	AbilityComponent->bIsAnyAbilityActive = true;

	bAbilityCommited = true;
	
	AbilityCastingStarted++;
	if (CheckStandalone())
		OnRep_AbilityCastingStarted();

	return bAbilityCommited;
}

bool UGASAbility::CanCommitAbility()
{
	return true;
}

void UGASAbility::CommitResources()
{

}

void UGASAbility::CastAbility()
{
	AbilityCastingEnded++;
	AbilityComponent->bIsAnyAbilityActive = false;
	if (CheckStandalone())
		OnRep_AbilityCastingEnded();

	Targeting->SetTraceRange(Range);
	Targeting->SetTraceArea(AbilityArea);

	Targeting->Execute();
	OnAbilityCastedDel.ExecuteIfBound();
	OnAbilityCasted();
}

void UGASAbility::CastAbilityInterval()
{

}

void UGASAbility::GotoState(class UGASAbilityState* NextState)
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

void UGASAbility::Tick(float DeltaTime)
{
	if (!bIsOnCooldown)
	{
		if (CheckStandalone())
			Targeting->Tick(DeltaTime);
		if (bUpdateHitLocationEveryFrame)
			Targeting->SingleLineTraceSetHitLocation();
	}

	if (CurrentState)
		CurrentState->Tick(DeltaTime);
}

bool UGASAbility::IsTickable() const
{
	if (IsTemplate())
		return false;

	return bShouldTick;
}

TStatId UGASAbility::GetStatId() const
{
	return GetStatID();
}

bool UGASAbility::IsNameStableForNetworking() const
{
	return bIsNameStable;
}

void UGASAbility::SetNetAddressable()
{
	bIsNameStable = true;
}

UWorld* UGASAbility::GetWorld() const
{
	if (AbilityComponent)
		return AbilityComponent->GetWorld(); //if ability component is ever null, something is very wrong!
	
	return Super::GetWorld();
}
void UGASAbility::SetHitLocation(const FVector& OriginIn, const FVector& HitLocationIn)
{
	HitLocation.HitCounter++;
	HitLocation.Origin = OriginIn;
	HitLocation.HitLocation = HitLocationIn;
	if (CheckStandalone())
		OnRep_HitLocation();
}

void UGASAbility::HitTarget(AActor* HitActor, const FVector& HitLocation, const FHitResult& Hit)
{
	OnAbilityTargetHit(HitActor, HitLocation, Hit);
}

void UGASAbility::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGASAbility, AbilityCastingStarted, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UGASAbility, AbilityCastingEnded, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UGASAbility, HitLocation, COND_SkipOwner);
}

void UGASAbility::OnRep_AbilityCastingStarted()
{
	OnAbilityCastingStarted();
}
void UGASAbility::OnRep_AbilityCastingEnded()
{
	OnAbilityCastingEnded();
}
void UGASAbility::OnRep_HitLocation()
{
	OnAbilityHit(HitLocation.Origin, HitLocation.HitLocation);
}
ENetMode UGASAbility::GetNetMode()
{
	if (!AbilityComponent)
		return ENetMode::NM_MAX;

	return AbilityComponent->GetNetMode();
}
bool UGASAbility::CheckStandalone()
{
	if (!AbilityComponent)
		return false;

	if (AbilityComponent->GetNetMode() == ENetMode::NM_Standalone
		|| AbilityComponent->GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return true;
	}
	return false;
}
void UGASAbility::EndCooldown_Implementation()
{
	bIsOnCooldown = false;
}