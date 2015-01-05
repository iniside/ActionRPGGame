// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GISItemData.h"
#include "GISGlobalTypes.h"

#include "../GSAbility.h"
#include "../GSAbilitiesComponent.h"

#include "GSAbilityItemWidget.h"

UGSAbilityItemWidget::UGSAbilityItemWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UGSAbilityItemWidget::InitializeItem()
{
	AbilityComponent = Cast<UGSAbilitiesComponent>(InventoryComponent);
}


float const UGSAbilityItemWidget::GetCurrentCooldownTime() const
{
	if (AbilityComponent && AbilityComponent->ActiveAbility)
	{
		return AbilityComponent->ActiveAbility->GetCurrentCooldownTime();
	}
	return 0;
}

float const UGSAbilityItemWidget::GetMaxCooldownTime() const
{
	if (AbilityComponent && AbilityComponent->ActiveAbility)
	{
		return AbilityComponent->ActiveAbility->GetCooldownTime();
	}
	return 0;
}