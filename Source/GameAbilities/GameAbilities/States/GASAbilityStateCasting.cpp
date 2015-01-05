// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateCasting.h"

UGASAbilityStateCasting::UGASAbilityStateCasting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateCasting::Tick(float DeltaSeconds)
{
	CurrentCastTime += DeltaSeconds;
	if (CurrentCastTime >= GetOuterAGASAbility()->CastTime)
	{
		CurrentCastTime = 0;
		ExecuteAbility();
	}
}

void UGASAbilityStateCasting::ExecuteAbility()
{
	GetOuterAGASAbility()->bIsBeingCast = false;
	GetOuterAGASAbility()->ExecuteAbility();
	
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateCasting::BeginState(UGASAbilityState* PrevState)
{
	GetOuterAGASAbility()->bIsBeingCast = true;
	
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCasting::EndState()
{
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCasting::BeginActionSequence()
{}
void UGASAbilityStateCasting::EndActionSequence()
{}