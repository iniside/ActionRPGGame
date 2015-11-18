// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbilityBase.h"
#include "GASAbilityStatePreparationWaitForInput.h"

UGASAbilityStatePreparationWaitForInput::UGASAbilityStatePreparationWaitForInput(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityStatePreparationWaitForInput::GetWorld() const
{
	return GetOuterUGASAbilityBase()->GetWorld();
}

void UGASAbilityStatePreparationWaitForInput::Tick(float DeltaSeconds)
{
}
void UGASAbilityStatePreparationWaitForInput::BeginState(UGASAbilityState* PrevState)
{
	//GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString("This ability is in preparation State"));

	//we will run any preparation actions, which should be executed at this stage, to
	//to provide ability with any base data, it needs to run.

	//GetOuterUGASAbility()->AbilityPreparationStart();

	//GetOuterUGASAbility()->SetTickEnabled(true);
	//GetOuterUGASAbility()->RunPreparationActions();
	//BeginActionSequence();
}
void UGASAbilityStatePreparationWaitForInput::EndState()
{
	//clean up.
	//GetOuterUGASAbility()->AbilityPreparationEnd();
	//GetOuterUGASAbility()->SetTickEnabled(false);
}
void UGASAbilityStatePreparationWaitForInput::BeginActionSequence()
{
	//GetOuterUGASAbility()->SetTickEnabled(false);
	//GetOuterUGASAbility()->GotoState(GetOuterUGASAbility()->ActivationState);
}
void UGASAbilityStatePreparationWaitForInput::EndActionSequence()
{
}