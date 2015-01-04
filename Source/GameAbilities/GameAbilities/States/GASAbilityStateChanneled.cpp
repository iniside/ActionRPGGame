// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateChanneled.h"

UGASAbilityStateChanneled::UGASAbilityStateChanneled(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
}

void UGASAbilityStateChanneled::Tick(float DeltaSeconds)
{
	CurrentCastTime += DeltaSeconds;
	if (CurrentCastTime >= GetOuterAGASAbility()->PeriodLenght
		&& GetOuterAGASAbility()->PeriodCount > CurrentPeriodCount)
	{
		CurrentCastTime = 0;
		CurrentPeriodCount += 1;
		ExecuteAbility();
	}
	else
	{
		CurrentPeriodCount = 0;
		ChannelFinished();
	}
}

void UGASAbilityStateChanneled::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbility();
	//GetOuterAGASAbility()->OnAbilityActivated();
	//GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneled::ChannelFinished()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneled::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneled::EndState()
{
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneled::BeginActionSequence()
{}
void UGASAbilityStateChanneled::EndActionSequence()
{}