// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilitySlotWidget.h"

#include "AMAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"

#include "ARAbilityBase.h"
#include "ARAbilityUIData.h"

float UARAbilitySlotWidget::GetActivationRemainingTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTime() : 0;
}
float UARAbilitySlotWidget::GetActivationRemainingTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetActivationCurrentTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTime() : 0;
}
float UARAbilitySlotWidget::GetActivationCurrentTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetActivationEndTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationEndTime() : 0;
}

float UARAbilitySlotWidget::GetCooldownRemainingTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTime() : 0;
}
float UARAbilitySlotWidget::GetCooldownRemainingTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetCooldownCurrentTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTime() : 0;
}
float UARAbilitySlotWidget::GetCooldownCurrentTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetCooldownEndTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownEndTime() : 0;
}