// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityStateCooldown.h"

UGASAbilityStateCooldown::UGASAbilityStateCooldown(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGASAbilityStateCooldown::Tick(float DeltaSeconds)
{
	GetOuterAGASAbility()->CurrentCooldownTime += DeltaSeconds;
	if (GetOuterAGASAbility()->CurrentCooldownTime >= GetOuterAGASAbility()->CooldownTime)
	{
		GetOuterAGASAbility()->CurrentCooldownTime = 0;
		FinishCooldown();
	}
}
void UGASAbilityStateCooldown::FinishCooldown()
{
	GetOuterAGASAbility()->bIsOnCooldown = false;
	//GetOuterAARAbility()->LeaveCooldown();
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->ActiveState);
}
void UGASAbilityStateCooldown::BeginState(UGASAbilityState* PrevState)
{
	GetOuterAGASAbility()->bIsOnCooldown = true;
	//GetOuterAARAbility()->EnterCooldown();
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(true);
}
void UGASAbilityStateCooldown::EndState()
{
	//GetOuterAARAbility()->bIsCooldownFinished = true;
	GetOuterAGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
}
void UGASAbilityStateCooldown::BeginActionSequence()
{
}
void UGASAbilityStateCooldown::EndActionSequence()
{
}