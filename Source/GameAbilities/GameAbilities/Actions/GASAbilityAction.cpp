// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityAction.h"

UGASAbilityAction::UGASAbilityAction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityAction::GetWorld() const
{
	return GetOuterUGASAbility()->GetWorld();
}


void UGASAbilityAction::Tick(float DeltaSecondsIn)
{

}
void UGASAbilityAction::Initialize()
{

}

void UGASAbilityAction::PreExecute()
{

}
void UGASAbilityAction::Execute()
{

}

void UGASAbilityAction::PostExecute()
{

}