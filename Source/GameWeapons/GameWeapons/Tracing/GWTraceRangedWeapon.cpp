// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameWeapons.h"
#include "DrawDebugHelpers.h"
#include "../GWWeapon.h"
#include "GWTraceRangedWeapon.h"

UGWTraceRangedWeapon::UGWTraceRangedWeapon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIgnoreSelf = true;
}