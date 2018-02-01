// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityWidget.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "AMAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"

#include "ARAbilityBase.h"
#include "ARAbilityUIData.h"
#include "Input/Reply.h"
#include "Input/Events.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ARGlobals.h"

float UARAbilityWidget::GetActivationRemainingTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTime() : 0;
}
float UARAbilityWidget::GetActivationRemainingTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationRemainingTimeNormalized() : 0;
}
float UARAbilityWidget::GetActivationCurrentTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTime() : 0;
}
float UARAbilityWidget::GetActivationCurrentTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationCurrentTimeNormalized() : 0;
}
float UARAbilityWidget::GetActivationEndTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetActivationEndTime() : 0;
}

float UARAbilityWidget::GetCooldownRemainingTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTime() : 0;
}
float UARAbilityWidget::GetCooldownRemainingTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownRemainingTimeNormalized() : 0;
}
float UARAbilityWidget::GetCooldownCurrentTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTime() : 0;
}
float UARAbilityWidget::GetCooldownCurrentTimeNormalized()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownCurrentTimeNormalized() : 0;
}
float UARAbilityWidget::GetCooldownEndTime()
{
	if (!AbilityManager)
		return 0;
	UGAAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->GetCooldownEndTime() : 0;
}
UTexture2D* UARAbilityWidget::GetIcon()
{
	if (Icon)
		return Icon;

	if (!AbilityManager)
		return nullptr;

	return nullptr;
	/*UARAbilityBase* Ability = AbilityManager->GetAbility(Group, AbilitySlot);
	return Ability ? Ability->UIData->Icon : nullptr;*/
}