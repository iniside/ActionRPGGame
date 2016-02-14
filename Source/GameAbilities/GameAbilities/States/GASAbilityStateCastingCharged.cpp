// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateCastingCharged.h"

UGASAbilityStateCastingCharged::UGASAbilityStateCastingCharged(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//CurrentCastTime = 0;
}

void UGASAbilityStateCastingCharged::Tick(float DeltaSeconds)
{
	//GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
	//CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->CastTime)
	//{
	//	GetOuterUGASAbility()->CurrentCastTime = 0;
	//	CurrentCastTime = 0;
	//	ExecuteAbility();
	//}
}

void UGASAbilityStateCastingCharged::ExecuteAbility()
{
	CurrentCastTime = 0;
	//GetOuterUGASAbility()->bIsBeingCast = false;
	//GetOuterUGASAbility()->ExecuteAbility();
	//
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateCastingCharged::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->OnAbilityActivated();
	//GetOuterUGASAbility()->bIsBeingCast = true;
	//GetOuterUGASAbility()->AbilityCastStart();
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCastingCharged::EndState()
{
	CurrentCastTime = 0;
	//GetOuterUGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCastingCharged::ExecuteActionSequence()
{}
void UGASAbilityStateCastingCharged::StopActionSequence()
{
//	GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	CurrentCastTime = 0;
//	GetOuterUGASAbility()->GotoActiveState();
}
void UGASAbilityStateCastingCharged::FinishActionSequence()
{
}