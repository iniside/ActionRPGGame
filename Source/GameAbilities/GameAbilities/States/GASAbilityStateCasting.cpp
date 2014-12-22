// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateCasting.h"

UGASAbilityStateCasting::UGASAbilityStateCasting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGASAbilityStateCasting::Tick(float DeltaSeconds)
{
	/*GetOuterAGASAbility()->CurrentCastTime += DeltaSeconds;
	if (GetOuterAGASAbility()->CurrentCastTime >= GetOuterAARAbility()->MaxCastTime)
	{
		GetOuterAGASAbility()->CurrentCastTime = 0;
		CastAbility();
	}*/
}

void UGASAbilityStateCasting::CastAbility()
{
	GetOuterAGASAbility()->ActivateAbility();
	//GetOuterAARAbility()->bIsCasting = false;
	//GetOuterAARAbility()->bIsCastingFinished = true;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->CooldownState);
}

void UGASAbilityStateCasting::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAARAbility()->bIsCasting = true;
	//GetOuterAARAbility()->bIsCastingFinished = false;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCasting::EndState()
{}
void UGASAbilityStateCasting::BeginActionSequence()
{}
void UGASAbilityStateCasting::EndActionSequence()
{}