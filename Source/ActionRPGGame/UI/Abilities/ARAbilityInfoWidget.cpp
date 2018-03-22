// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityInfoWidget.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "Abilities/GAAbilityBase.h"
#include "ARPlayerController.h"
#include "Abilities/ARAbilityBase.h"
#include "Abilities/ARAbilityManagerComponent.h"

float UARAbilityInfoWidget::GetRemainingCooldown() const
{
	if (!AbilityManager.IsValid())
		return 0;

	UGAAbilityBase* Ability = AbilityManager->GetAbility(AbilityGroup, AbilitySlot);
	if (Ability)
	{
		return Ability->GetCooldownRemainingTimeNormalized();
	}
	return 0;
}