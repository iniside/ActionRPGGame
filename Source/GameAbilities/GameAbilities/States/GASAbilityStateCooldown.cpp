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
}
void UGASAbilityStateCooldown::FinishCooldown()
{
	GetOuterUGASAbilityBase()->bIsOnCooldown = false;
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->DefaultState);
}
void UGASAbilityStateCooldown::BeginState(UGASAbilityState* PrevState)
{
	if (GetOuterUGASAbilityBase()->ApplyCooldownEffect())
	{
		GetOuterUGASAbilityBase()->bIsOnCooldown = true;
	}
	else
	{
		GetOuterUGASAbilityBase()->bIsOnCooldown = false;
		GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->DefaultState);
	}
	
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());
}
void UGASAbilityStateCooldown::EndState()
{
	//GetOuterUGASAbilityBase()->bIsOnCooldown = false;
	//GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->DefaultState);
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
}
void UGASAbilityStateCooldown::ExecuteActionSequence()
{
}
void UGASAbilityStateCooldown::StopActionSequence()
{
}
void UGASAbilityStateCooldown::FinishActionSequence()
{
}