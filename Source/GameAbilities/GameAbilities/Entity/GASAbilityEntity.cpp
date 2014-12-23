// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"
#include "../GASAbility.h"
#include "GASAbilityEntity.h"

UGASAbilityEntity::UGASAbilityEntity(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
UWorld* UGASAbilityEntity::GetWorld() const
{
	return GetOuterAGASAbility()->GetWorld();
}
