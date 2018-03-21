// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityListWidget.h"
#include "ARAbilityListSlotDragWidget.h"

#include "ARPlayerController.h"

void UARAbilityListWidget::AddItem(TSubclassOf<class UARAbilityListSlotDragWidget> DragWidgetClass, const FGameplayTag& Ability)
{
	UARAbilityListSlotDragWidget* Item = CreateWidget<UARAbilityListSlotDragWidget>(ARPC.Get(), DragWidgetClass);
	Item->AbilityTag = Ability;
	Items.Add(Item);

	ItemContainer->AddChild(Item);
}
