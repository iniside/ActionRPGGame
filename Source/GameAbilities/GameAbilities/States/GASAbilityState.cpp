// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "GASAbilityState.h"

UGASAbilityState::UGASAbilityState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityState::GetWorld() const
{
	return GetOuterUGASAbilityBase()->GetWorld();
}

void UGASAbilityState::Tick(float DeltaSeconds)
{
}
void UGASAbilityState::BeginState(UGASAbilityState* PrevState)
{
}
void UGASAbilityState::EndState()
{
}
void UGASAbilityState::ExecuteActionSequence()
{
}
void UGASAbilityState::StopActionSequence()
{
}
void UGASAbilityState::FinishActionSequence()
{
}