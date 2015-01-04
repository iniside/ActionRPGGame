// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateCastingBase.h"

UGASAbilityStateCastingBase::UGASAbilityStateCastingBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateCastingBase::Tick(float DeltaSeconds)
{
	CurrentCastTime += DeltaSeconds;
	if (CurrentCastTime >= GetOuterAGASAbility()->CastTime)
	{
		CurrentCastTime = 0;
		ExecuteAbility();
	}
}

void UGASAbilityStateCastingBase::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbility();
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateCastingBase::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCastingBase::EndState()
{
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCastingBase::BeginActionSequence()
{}
void UGASAbilityStateCastingBase::EndActionSequence()
{}