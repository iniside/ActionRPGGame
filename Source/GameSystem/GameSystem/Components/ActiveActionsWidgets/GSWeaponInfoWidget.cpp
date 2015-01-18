// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSActiveActionsComponent.h"
#include "../../Items/GSItemWeaponInfo.h"
#include "../../Items/GSItemWeaponRangedInfo.h"
#include "GSWeaponInfoWidget.h"

UGSWeaponInfoWidget::UGSWeaponInfoWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

int32 UGSWeaponInfoWidget::GetCurrentMagazineAmmo() const
{
	if (UGSItemWeaponRangedInfo* weap = Cast<UGSItemWeaponRangedInfo>(CurrentWeapon))
	{
		return weap->GetCurrentMagainzeAmmo();
	}
	return 0;
}

int32 UGSWeaponInfoWidget::GetCurrentTotalAmmo() const
{
	if (UGSItemWeaponRangedInfo* weap = Cast<UGSItemWeaponRangedInfo>(CurrentWeapon))
	{
		return weap->GetCurrentTotalAmmo();
	}
	return 0;
}