// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAEffect.h"

UGAEffect::UGAEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

bool UGAEffect::OnEffectApplied_Implementation(const FGAEffectContext& Context)
{
	return false;
}