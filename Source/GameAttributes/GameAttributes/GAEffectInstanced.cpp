// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffectInstanced.h"

UGAEffectInstanced::UGAEffectInstanced(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

bool UGAEffectInstanced::OnEffectApplied_Implementation(const FGAEffectContext& Context)
{
	return false;
}