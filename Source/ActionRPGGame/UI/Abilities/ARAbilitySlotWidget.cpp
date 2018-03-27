// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilitySlotWidget.h"

#include "AMAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"

#include "ARAbilityBase.h"
#include "ARAbilityUIData.h"

void UARAbilitySlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CooldownBar->SetPercent(GetCooldownRemainingTimeNormalized());
}

float UARAbilitySlotWidget::GetActivationRemainingTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTime() : 0;
}
float UARAbilitySlotWidget::GetActivationRemainingTimeNormalized()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetActivationCurrentTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTime() : 0;
}
float UARAbilitySlotWidget::GetActivationCurrentTimeNormalized()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetActivationEndTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationEndTime() : 0;
}

float UARAbilitySlotWidget::GetCooldownRemainingTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTime() : 0;
}
float UARAbilitySlotWidget::GetCooldownRemainingTimeNormalized()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetCooldownCurrentTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTime() : 0;
}
float UARAbilitySlotWidget::GetCooldownCurrentTimeNormalized()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTimeNormalized() : 0;
}
float UARAbilitySlotWidget::GetCooldownEndTime()
{
	if (!AbilityManager.IsValid())
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownEndTime() : 0;
}