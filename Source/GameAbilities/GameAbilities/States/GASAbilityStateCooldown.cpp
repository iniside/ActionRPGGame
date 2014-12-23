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
	//GetOuterAARAbility()->CurrentCooldownTime += DeltaSeconds;
	//if (GetOuterAARAbility()->CurrentCooldownTime >= GetOuterAARAbility()->MaxRecastTime)
	//{
	//	GetOuterAARAbility()->CurrentCooldownTime = 0;
		FinishCooldown();
	//}
}
void UGASAbilityStateCooldown::FinishCooldown()
{
	//GetOuterAARAbility()->LeaveCooldown();
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->ActiveState);
}
void UGASAbilityStateCooldown::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterAARAbility()->bIsCooldownFinished = false;
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