// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateChanneled.h"

UGASAbilityStateChanneled::UGASAbilityStateChanneled(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASAbilityStateChanneled::Tick(float DeltaSeconds)
{
}

void UGASAbilityStateChanneled::ExecuteAbility()
{
	GetOuterUGASAbilityBase()->OnAbilityExecuted();

}
void UGASAbilityStateChanneled::ExecuteChannel()
{
	GetOuterUGASAbilityBase()->OnAbilityExecuted();
}
void UGASAbilityStateChanneled::ChannelFinished()
{
}

void UGASAbilityStateChanneled::BeginState(UGASAbilityState* PrevState)
{
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());

	if (!GetOuterUGASAbilityBase()->ApplyActivationEffect())
	{
		//GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
	}
}
void UGASAbilityStateChanneled::EndState()
{
	UE_LOG(GameAbilities, Log, TEXT("Ending State: %s"), *GetName());
	GetOuterUGASAbilityBase()->NativeFinishExecution();
}
void UGASAbilityStateChanneled::ExecuteActionSequence()
{}
void UGASAbilityStateChanneled::StopActionSequence()
{
	GetOuterUGASAbilityBase()->OnNativeStopAbility();
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->CooldownState);
}
void UGASAbilityStateChanneled::FinishActionSequence()
{
}