// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityMods.h"

UGASAbilityMods::UGASAbilityMods(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityMods::GetWorld() const
{
	return GetOuterAGASAbility()->GetWorld();
}
