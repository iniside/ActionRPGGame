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
	GetOuterAGASAbility()->ExecuteAbility();
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateCastingCharged::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCastingCharged::EndState()
{
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCastingCharged::BeginActionSequence()
{}
void UGASAbilityStateCastingCharged::EndActionSequence()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	CurrentCastTime = 0;
	//do not go to next state.
	//allthough we could go to cooldown state if we did not finished charging
	//and add grace period in which cooldown will be shorter.
}