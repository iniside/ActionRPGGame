// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"
#include "../GSAbility.h"
#include "../GSAbilitiesComponent.h"
#include "GSAbilityWidget.h"

UGSAbilityWidget::UGSAbilityWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityIndex = INDEX_NONE;
}
void UGSAbilityWidget::UpdateWidgetData()
{
	if (AbilityIndex != INDEX_NONE)
		AbilityPtrCache = AbilityComponent->GetGSAbility(AbilityIndex);
}
float UGSAbilityWidget::GetAbilityCooldown() const
{
	if (AbilityIndex != INDEX_NONE)
	{
		return AbilityComponent->GetGASAbility(AbilityIndex)->CurrentRechargeTime;
	}
	return 0;
}
UTexture2D* UGSAbilityWidget::GetAbilityIcon() const
{
	if (AbilityPtrCache)
		return AbilityPtrCache->AbilityIcon;
	return nullptr;
}