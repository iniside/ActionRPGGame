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
		GetOuterAGASAbility()->CurrentCastTime = 0;
		CurrentPeriodCount = 0;
		ChannelFinished();
	}
}

void UGASAbilityStateChanneled::ExecuteAbility()
{
	GetOuterAGASAbility()->ExecuteAbility();

}

void UGASAbilityStateChanneled::ChannelFinished()
{
	GetOuterAGASAbility()->bIsBeingCast = false;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateChanneled::BeginState(UGASAbilityState* PrevState)
{
	GetOuterAGASAbility()->bIsBeingCast = true;
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