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
//	GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
//	CurrentCastTime += DeltaSeconds;
//	if (CurrentCastTime >= GetOuterUGASAbility()->PeriodLenght
//		&& GetOuterUGASAbility()->PeriodCount > CurrentPeriodCount)
//	{
//		CurrentCastTime = 0;
//		CurrentPeriodCount += 1;
//		ExecuteAbility();
//	}
//	else if (CurrentPeriodCount >= GetOuterUGASAbility()->PeriodCount)
//	{
//		CurrentPeriodCount = 0;
//		GetOuterUGASAbility()->CurrentCastTime = 0;
//		ChannelFinished();
//	}
}

void UGASAbilityStateChanneledCharged::ExecuteAbility()
{
	//GetOuterUGASAbility()->ExecuteAbility();
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
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
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
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->SetActorTickEnabled(false);
	//CurrentCastTime = 0;
	//CurrentPeriodCount = 0;
	//GetOuterUGASAbility()->GotoActiveState();
}