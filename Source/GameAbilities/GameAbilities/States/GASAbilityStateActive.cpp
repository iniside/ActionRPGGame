// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbility.h"

#include "GASAbilityStateActive.h"

UGASAbilityStateActive::UGASAbilityStateActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
void UGASAbilityStateActive::BeginState(UGASAbilityState* PrevState)
{
}
void UGASAbilityStateActive::EndState()
{
}
void UGASAbilityStateActive::BeginActionSequence()
{
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->ActivationState);
}
void UGASAbilityStateActive::EndActionSequence()
{
}