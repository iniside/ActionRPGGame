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
#include "../ARPlayerController.h"
#include "ARGlobals.h"

void UARAbilityWidget::NativePreConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		UIComponent = MyPC->UIComponent;
		AbilityManager = MyPC->AbilityManager;
	}
	Super::NativePreConstruct();
}
void UARAbilityWidget::NativeConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		UIComponent = MyPC->UIComponent;
		AbilityManager = MyPC->AbilityManager;
	}
	Super::NativeConstruct();
}


UTexture2D* UARAbilityWidget::GetIcon()
{
	if (Icon)
		return Icon;

	if (!AbilityManager)
		return nullptr;

	//return nullptr;
	UARAbilityBase* Ability = nullptr;// Cast<UARAbilityBase>(AbilityManager->GetAbility(Group, AbilitySlot));
	return Ability ? Ability->UIData->Icon : nullptr;
}