// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityStatePreparation.h"

UGASAbilityStatePreparation::UGASAbilityStatePreparation(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityStatePreparation::GetWorld() const
{
	return GetOuterUGASAbility()->GetWorld();
}

void UGASAbilityStatePreparation::Tick(float DeltaSeconds)
{
}
void UGASAbilityStatePreparation::BeginState(UGASAbilityState* PrevState)
{
	//GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString("This ability is in preparation State"));

	//we will run any preparation actions, which should be executed at this stage, to
	//to provide ability with any base data, it needs to run.

	//GetOuterUGASAbility()->AbilityPreparationStart();

	//GetOuterUGASAbility()->SetActorTickEnabled(true);
	//GetOuterUGASAbility()->RunPreparationActions();
	//BeginActionSequence();
	//if there is no need for something special, like display some targeting helpers
	//we run those action and move immidietly to next state
	//which means we would call BeginActionSequence().
	//if otherwise we hold until player will send input again.
	//for testing right now assume that we want player to press input twice.
}
void UGASAbilityStatePreparation::EndState()
{
	//clean up.
	//GetOuterUGASAbility()->AbilityPreparationEnd();
	//GetOuterUGASAbility()->SetActorTickEnabled(false);
}
void UGASAbilityStatePreparation::BeginActionSequence()
{
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->ActivationState);
}
void UGASAbilityStatePreparation::EndActionSequence()
{
}