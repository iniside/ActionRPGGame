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
	else if (CurrentPeriodCount >= GetOuterAGASAbility()->PeriodCount)
	{
		CurrentPeriodCount = 0;
		ChannelFinished();
	}
}

void UGASAbilityStateChanneledCharged::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbility();
}

void UGASAbilityStateChanneledCharged::ChannelFinished()
{
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->bIsBeingCast = false;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->SetActorTickEnabled(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledCharged::BeginState(UGASAbilityState* PrevState)
{
	GetOuterAGASAbility()->bIsBeingCast = true;
	GetOuterAGASAbility()->AbilityCastStart();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneledCharged::EndState()
{
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledCharged::BeginActionSequence()
{}
void UGASAbilityStateChanneledCharged::EndActionSequence()
{
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->SetActorTickEnabled(false);
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
	GetOuterAGASAbility()->GotoActiveState();
}