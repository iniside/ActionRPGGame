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
	GetOuterUGASAbility()->CurrentRechargeTime += DeltaSeconds;
	if (GetOuterUGASAbility()->CurrentRechargeTime >= GetOuterUGASAbility()->RechargeTime
		&& GetOuterUGASAbility()->bIsOnCooldown)
	{
		GetOuterUGASAbility()->CurrentRechargeTime = 0;
		FinishCooldown();
	}
}
void UGASAbilityStateCooldown::FinishCooldown()
{
	GetOuterUGASAbility()->bIsOnCooldown = false;
	if (GetOuterUGASAbility()->GetNetMode() == ENetMode::NM_DedicatedServer)
		GetOuterUGASAbility()->EndCooldown();

	GetOuterUGASAbility()->SetTickEnabled(false);
	GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->ActiveState);
}
void UGASAbilityStateCooldown::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->bIsOnCooldown = true;
	//GetOuterAARAbility()->EnterCooldown();
	GetOuterUGASAbility()->bIsOnCooldown = true;
	GetOuterUGASAbility()->SetTickEnabled(true);
}
void UGASAbilityStateCooldown::EndState()
{
	//GetOuterAARAbility()->bIsCooldownFinished = true;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);
}
void UGASAbilityStateCooldown::BeginActionSequence()
{
}
void UGASAbilityStateCooldown::EndActionSequence()
{
}