// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateChanneledLocked.h"

UGASAbilityStateChanneledLocked::UGASAbilityStateChanneledLocked(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
	CurrentPeriodCount = 0;
}

void UGASAbilityStateChanneledLocked::Tick(float DeltaSeconds)
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
	//else if(CurrentPeriodCount >= GetOuterUGASAbility()->PeriodCount)
	//{
	//	GetOuterUGASAbility()->CurrentCastTime = 0;
	//	CurrentPeriodCount = 0;
	//	ChannelFinished();
	//}
}
void UGASAbilityStateChanneledLocked::ChannelStarted()
{

}
void UGASAbilityStateChanneledLocked::ExecuteAbility()
{
//	GetOuterUGASAbility()->ExecuteAbility();

}

void UGASAbilityStateChanneledLocked::ChannelFinished()
{
	//GetOuterUGASAbility()->bIsBeingCast = false;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledLocked::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->bIsBeingCast = true;
	//GetOuterUGASAbility()->AbilityCastStart();
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateChanneledLocked::EndState()
{
//	GetOuterUGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledLocked::BeginActionSequence()
{}
void UGASAbilityStateChanneledLocked::EndActionSequence()
{}