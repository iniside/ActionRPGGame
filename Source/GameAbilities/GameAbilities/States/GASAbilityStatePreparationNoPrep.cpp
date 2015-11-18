// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "GASAbilityStatePreparationNoPrep.h"

UGASAbilityStatePreparationNoPrep::UGASAbilityStatePreparationNoPrep(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityStatePreparationNoPrep::GetWorld() const
{
	return GetOuterUGASAbilityBase()->GetWorld();
}

void UGASAbilityStatePreparationNoPrep::Tick(float DeltaSeconds)
{
}
void UGASAbilityStatePreparationNoPrep::BeginState(UGASAbilityState* PrevState)
{
	BeginActionSequence();
}
void UGASAbilityStatePreparationNoPrep::EndState()
{
}
void UGASAbilityStatePreparationNoPrep::BeginActionSequence()
{
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->ActivationState);
}
void UGASAbilityStatePreparationNoPrep::EndActionSequence()
{
}