// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateChanneledCharged.h"

UGASAbilityStateChanneledCharged::UGASAbilityStateChanneledCharged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateChanneledCharged::Tick(float DeltaSeconds)
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

void UGASAbilityStateChanneledCharged::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbility();
	//GetOuterAGASAbility()->OnAbilityActivated();
	//GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledCharged::ChannelFinished()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledCharged::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAGASAbility()->OnAbilityActivated();
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneledCharged::EndState()
{
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledCharged::BeginActionSequence()
{}
void UGASAbilityStateChanneledCharged::EndActionSequence()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
}