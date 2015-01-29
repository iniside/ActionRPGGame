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
	//GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
	//CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->PeriodLenght
	//	&& GetOuterUGASAbility()->PeriodCount > CurrentPeriodCount)
	//{
	//	CurrentCastTime = 0;
	//	CurrentPeriodCount += 1;
	//	ExecuteAbility();
	//}
	//else if (CurrentPeriodCount >= GetOuterUGASAbility()->PeriodCount)
	//{
	//	CurrentPeriodCount = 0;
	//	GetOuterUGASAbility()->CurrentCastTime = 0;
	//	ChannelFinished();
	//}
}

void UGASAbilityStateChanneledChargedLocked::ExecuteAbility()
{
	//GetOuterUGASAbility()->ExecuteAbilityPeriod();
}
void UGASAbilityStateChanneledChargedLocked::ChannelStarted()
{
	//GetOuterUGASAbility()->ExecuteAbility();
}
void UGASAbilityStateChanneledChargedLocked::ChannelFinished()
{
	//GetOuterUGASAbility()->CurrentCastTime = 0;
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->bIsBeingCast = false;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->SetActorTickEnabled(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledChargedLocked::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->bIsBeingCast = true;
	//ChannelStarted();
	//GetOuterUGASAbility()->AbilityCastStart();
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneledChargedLocked::EndState()
{
	//GetOuterUGASAbility()->CurrentCastTime = 0;
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledChargedLocked::BeginActionSequence()
{}
void UGASAbilityStateChanneledChargedLocked::EndActionSequence()
{
	//GetOuterUGASAbility()->CurrentCastTime = 0;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->SetActorTickEnabled(false);
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->GotoActiveState();
}