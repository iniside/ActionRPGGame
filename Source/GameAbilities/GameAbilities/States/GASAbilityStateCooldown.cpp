// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "GASAbilityStateCooldown.h"

UGASAbilityStateCooldown::UGASAbilityStateCooldown(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGASAbilityStateCooldown::Tick(float DeltaSeconds)
{
	//GetOuterUGASAbility()->CurrentRechargeTime += DeltaSeconds;
	//if (GetOuterUGASAbility()->CurrentRechargeTime >= GetOuterUGASAbility()->RechargeTime
	//	&& GetOuterUGASAbility()->bIsOnCooldown)
	//{
	//	GetOuterUGASAbility()->CurrentRechargeTime = 0;
	//	FinishCooldown();
	//}
}
void UGASAbilityStateCooldown::FinishCooldown()
{
	//GetOuterUGASAbility()->bIsOnCooldown = false;
	//if (GetOuterUGASAbility()->GetNetMode() == ENetMode::NM_DedicatedServer)
	//	GetOuterUGASAbility()->EndCooldown();

	//GetOuterUGASAbility()->SetTickEnabled(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->ActiveState);
	FTimerManager& TimerManager = GetOuterUGASAbilityBase()->GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(CooldownTimerDelegate);
	GetOuterUGASAbilityBase()->bIsOnCooldown = false;
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->DefaultState);
}
void UGASAbilityStateCooldown::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->bIsOnCooldown = true;
	//GetOuterAARAbility()->EnterCooldown();
	GetOuterUGASAbilityBase()->bIsOnCooldown = true;
	//GetOuterUGASAbility()->SetTickEnabled(true);
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());
	FTimerManager& TimerManager = GetOuterUGASAbilityBase()->GetWorld()->GetTimerManager();
	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UGASAbilityStateCooldown::FinishCooldown);
	TimerManager.SetTimer(CooldownTimerDelegate, del, Cooldown, false, Cooldown);
	//FinishCooldown();
}
void UGASAbilityStateCooldown::EndState()
{
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
	//GetOuterAARAbility()->bIsCooldownFinished = true;
	//GetOuterUGASAbility()->PrimaryActorTick.SetTickFunctionEnable(false);

}
void UGASAbilityStateCooldown::BeginActionSequence()
{
}
void UGASAbilityStateCooldown::EndActionSequence()
{
}