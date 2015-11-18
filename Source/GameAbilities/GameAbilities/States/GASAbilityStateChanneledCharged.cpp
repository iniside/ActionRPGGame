// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateChanneledCharged.h"

UGASAbilityStateChanneledCharged::UGASAbilityStateChanneledCharged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateChanneledCharged::Tick(float DeltaSeconds)
{
	//GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
	CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->CastTime)
	//	//&& GetOuterUGASAbility()->PeriodCount > CurrentPeriodCount)
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

void UGASAbilityStateChanneledCharged::ExecuteAbility()
{
	//GetOuterUGASAbility()->CastAbility();
}

void UGASAbilityStateChanneledCharged::ChannelFinished()
{
//	//CurrentCastTime = 0;
//	//CurrentPeriodCount = 0;
//	//GetOuterUGASAbility()->bIsBeingCast = false;
//	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
//	//GetOuterUGASAbility()->SetActorTickEnabled(false);
//	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateChanneledCharged::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->bIsBeingCast = true;
	//GetOuterUGASAbility()->AbilityCastStart();
	//GetOuterUGASAbility()->SetTickEnabled(true);
}
void UGASAbilityStateChanneledCharged::EndState()
{
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateChanneledCharged::BeginActionSequence()
{}
void UGASAbilityStateChanneledCharged::EndActionSequence()
{
	//GetOuterUGASAbility()->SetTickEnabled(false);
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}