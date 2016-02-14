// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateCasting.h"

UGASAbilityStateCasting::UGASAbilityStateCasting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASAbilityStateCasting::Tick(float DeltaSeconds)
{
}

void UGASAbilityStateCasting::ExecuteAbility()
{
}

void UGASAbilityStateCasting::BeginState(UGASAbilityState* PrevState)
{
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());

	if (!GetOuterUGASAbilityBase()->ApplyActivationEffect())
	{
		GetOuterUGASAbilityBase()->OnAbilityExecutedNative();
		//GetOuterUGASAbilityBase()->NativeFinishExecution();
		//GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
	}
	else
	{
		GetOuterUGASAbilityBase()->OnAbilityExecutedNative();
		
	}
}
void UGASAbilityStateCasting::EndState()
{
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
	GetOuterUGASAbilityBase()->NativeFinishExecution();
}
void UGASAbilityStateCasting::ExecuteActionSequence()
{
}
void UGASAbilityStateCasting::StopActionSequence()
{}
void UGASAbilityStateCasting::FinishActionSequence()
{}