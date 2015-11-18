// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateCastingBase.h"

UGASAbilityStateCastingBase::UGASAbilityStateCastingBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastTime = 0;
}

void UGASAbilityStateCastingBase::Tick(float DeltaSeconds)
{
	//CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->CastTime)
	//{
	//	CurrentCastTime = 0;
	//	ExecuteAbility();
	//}
}

void UGASAbilityStateCastingBase::ExecuteAbility()
{
	//GetOuterUGASAbility()->ExecuteAbility();
	//GetOuterUGASAbility()->OnAbilityActivated();
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateCastingBase::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->OnAbilityActivated();
//	GetOuterUGASAbility()->AbilityCastStart();
//	GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCastingBase::EndState()
{
	//GetOuterUGASAbility()->AbilityCastEnd();
}
void UGASAbilityStateCastingBase::BeginActionSequence()
{}
void UGASAbilityStateCastingBase::EndActionSequence()
{}