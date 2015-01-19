// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "GSWeaponRanged.h"

AGSWeaponRanged::AGSWeaponRanged(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	WeaponType = EGSWeaponType::Ranged;
}
