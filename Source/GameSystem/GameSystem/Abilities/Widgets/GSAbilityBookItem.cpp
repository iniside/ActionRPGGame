// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSAbilitiesComponent.h"
#include "../../GSPlayerController.h"
#include "../GSAbility.h"
#include "GSAbilityBookItem.h"

UGSAbilityBookItem::UGSAbilityBookItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UTexture2D* UGSAbilityBookItem::GetAbilityIcon() const
{
	if (SlotInfo.AbilityClass)
	{
		return SlotInfo.AbilityClass->GetDefaultObject<UGSAbility>()->AbilityIcon;
	}
	return nullptr;
}