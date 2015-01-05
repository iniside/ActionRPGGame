// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateCastingCharged.h"

UGASAbilityStateCastingCharged::UGASAbilityStateCastingCharged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateCastingCharged::Tick(float DeltaSeconds)
{
	CurrentCastTime += DeltaSeconds;
	if (CurrentCastTime >= GetOuterAGASAbility()->CastTime)
	{
		CurrentCastTime = 0;
		ExecuteAbility();
	}
}

void UGASAbilityStateCastingCharged::ExecuteAbility()
{
	CurrentCastTime = 0;
	GetOuterAGASAbility()->bIsBeingCast = false;
	GetOuterAGASAbility()->ExecuteAbility();
	
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateCastingCharged::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->bIsBeingCast = true;
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCastingCharged::EndState()
{
	CurrentCastTime = 0;
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCastingCharged::BeginActionSequence()
{}
void UGASAbilityStateCastingCharged::EndActionSequence()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	CurrentCastTime = 0;
	GetOuterAGASAbility()->GotoActiveState();
}