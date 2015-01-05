// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateChanneledChargedLocked.h"

UGASAbilityStateChanneledChargedLocked::UGASAbilityStateChanneledChargedLocked(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateChanneledChargedLocked::Tick(float DeltaSeconds)
{
	GetOuterAGASAbility()->CurrentCastTime += DeltaSeconds;
	CurrentCastTime += DeltaSeconds;
	if (CurrentCastTime >= GetOuterAGASAbility()->PeriodLenght
		&& GetOuterAGASAbility()->PeriodCount > CurrentPeriodCount)
	{
		CurrentCastTime = 0;
		CurrentPeriodCount += 1;
		ExecuteAbility();
	}
	else if (CurrentPeriodCount >= GetOuterAGASAbility()->PeriodCount)
	{
		CurrentPeriodCount = 0;
		GetOuterAGASAbility()->CurrentCastTime = 0;
		ChannelFinished();
	}
}

void UGASAbilityStateChanneledChargedLocked::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbilityPeriod();
}
void UGASAbilityStateChanneledChargedLocked::ChannelStarted()
{
	GetOuterAGASAbility()->ExecuteAbility();
}
void UGASAbilityStateChanneledChargedLocked::ChannelFinished()
{
	GetOuterAGASAbility()->CurrentCastTime = 0;
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->bIsBeingCast = false;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->SetActorTickEnabled(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledChargedLocked::BeginState(UGASAbilityState* PrevState)
{
	GetOuterAGASAbility()->bIsBeingCast = true;
	ChannelStarted();
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneledChargedLocked::EndState()
{
	GetOuterAGASAbility()->CurrentCastTime = 0;
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledChargedLocked::BeginActionSequence()
{}
void UGASAbilityStateChanneledChargedLocked::EndActionSequence()
{
	GetOuterAGASAbility()->CurrentCastTime = 0;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->SetActorTickEnabled(false);
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->GotoActiveState();
}