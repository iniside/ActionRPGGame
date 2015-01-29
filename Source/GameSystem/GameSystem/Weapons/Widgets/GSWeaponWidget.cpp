// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../../Items/GSItemWeaponInfo.h"

#include "GSWeaponWidget.h"

UGSWeaponWidget::UGSWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UTexture2D* UGSWeaponWidget::GetWeaponIcon() const
{
	if (CurrentWeapon)
		return CurrentWeapon->GetImage();

	return nullptr;
}