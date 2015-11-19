// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "../GASAbilityBase.h"

#include "GASAbilityStateActive.h"

UGASAbilityStateActive::UGASAbilityStateActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
void UGASAbilityStateActive::BeginState(UGASAbilityState* PrevState)
{
	UE_LOG(GameAbilities, Log, TEXT("Begining State: %s"), *GetName());
}
void UGASAbilityStateActive::EndState()
{
}
void UGASAbilityStateActive::BeginActionSequence()
{
	UE_LOG(GameAbilities, Log, TEXT("BBeginActionSequence in: %s go to state: %s"), *GetName(), *GetOuterUGASAbilityBase()->ActivationState->GetName());
	GetOuterUGASAbilityBase()->GotoState(GetOuterUGASAbilityBase()->ActivationState);
}
void UGASAbilityStateActive::EndActionSequence()
{
}