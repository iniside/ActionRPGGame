// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityStatePreparationNoPrep.h"

UGASAbilityStatePreparationNoPrep::UGASAbilityStatePreparationNoPrep(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityStatePreparationNoPrep::GetWorld() const
{
	return GetOuterAGASAbility()->GetWorld();
}

void UGASAbilityStatePreparationNoPrep::Tick(float DeltaSeconds)
{
}
void UGASAbilityStatePreparationNoPrep::BeginState(UGASAbilityState* PrevState)
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString("This ability is in preparation State"));

	//we will run any preparation actions, which should be executed at this stage, to
	//to provide ability with any base data, it needs to run.

	GetOuterAGASAbility()->AbilityPreparationStart();

	GetOuterAGASAbility()->SetActorTickEnabled(true);
	GetOuterAGASAbility()->RunPreparationActions();
	//BeginActionSequence();
	//if there is no need for something special, like display some targeting helpers
	//we run those action and move immidietly to next state
	//which means we would call BeginActionSequence().
	//if otherwise we hold until player will send input again.
	//for testing right now assume that we want player to press input twice.
}
void UGASAbilityStatePreparationNoPrep::EndState()
{
	//clean up.
	GetOuterAGASAbility()->AbilityPreparationEnd();
	GetOuterAGASAbility()->SetActorTickEnabled(false);
}
void UGASAbilityStatePreparationNoPrep::BeginActionSequence()
{
	GetOuterAGASAbility()->GotoState(GetOuterAGASAbility()->ActivationState);
}
void UGASAbilityStatePreparationNoPrep::EndActionSequence()
{
}