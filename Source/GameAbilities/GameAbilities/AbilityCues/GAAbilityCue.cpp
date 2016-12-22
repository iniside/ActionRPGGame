// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GAAbilityBase.h"
#include "GAAbilityCue.h"

UGAAbilityCue::UGAAbilityCue(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGAAbilityCue::GetWorld() const
{
	return GetOuterUGAAbilityBase()->GetWorld();
}
APawn* UGAAbilityCue::GetPawnOwner() const
{
	return GetOuterUGAAbilityBase()->POwner;
}