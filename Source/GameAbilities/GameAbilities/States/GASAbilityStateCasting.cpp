// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateCasting.h"

UGASAbilityStateCasting::UGASAbilityStateCasting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//CurrentCastTime = 0;
}

void UGASAbilityStateCasting::Tick(float DeltaSeconds)
{
	//GetOuterUGASAbility()->CurrentCastTime += DeltaSeconds;
	//CurrentCastTime += DeltaSeconds;
	//if (CurrentCastTime >= GetOuterUGASAbility()->CastTime)
	//{
	//	GetOuterUGASAbility()->CurrentCastTime = 0;
	//	CurrentCastTime = 0;
	//	GetOuterUGASAbility()->SetTickEnabled(false);
	//	ExecuteAbility();
	//}
}

void UGASAbilityStateCasting::ExecuteAbility()
{
	//GetOuterUGASAbility()->bIsBeingCast = false;
	//GetOuterUGASAbility()->ExecuteAbility();
	//GetOuterUGASAbility()->CastAbility();
	//GetOuterUGASAbility()->SetTickEnabled(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->CooldownState);
}

void UGASAbilityStateCasting::BeginState(UGASAbilityState* PrevState)
{
	//GetOuterUGASAbility()->CurrentCastTime = 0;
	//if (GetOuterUGASAbility()->CommitAbility())
	//{
	//	GetOuterUGASAbility()->SetTickEnabled(true);
	//}
	//else
	//{
	//	//can commit ability, back to active state.
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());
		GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
	//}
	
}
void UGASAbilityStateCasting::EndState()
{
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
	GetOuterUGASAbilityBase()->OnAbilityExecutedNative();
	GetOuterUGASAbilityBase()->NativeFinishExecution();
}
void UGASAbilityStateCasting::BeginActionSequence()
{}
void UGASAbilityStateCasting::EndActionSequence()
{}