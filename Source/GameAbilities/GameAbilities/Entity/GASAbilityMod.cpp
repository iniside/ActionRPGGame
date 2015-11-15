// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityMod.h"

UGASAbilityMod::UGASAbilityMod(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityMod::GetWorld() const
{
	return GetOuterUGASAbility()->GetWorld();
}
