// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "IFInventoryComponent.h"

void UIFItemWidget::OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	NetIndex = InNetIndex;
	LocalIndex = InLocalIndex;
	BP_OnItemCreated(Item);
}

void UIFItemWidget::OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	BP_OnItemChanged(const_cast<UIFItemBase*>(Item));
}

void UIFItemWidget::OnItemRemoved(uint8 InNetIndex, uint8 InLocalIndex, class UIFItemBase* Item)
{
	BP_OnItemRemoved(const_cast<UIFItemBase*>(Item));
}