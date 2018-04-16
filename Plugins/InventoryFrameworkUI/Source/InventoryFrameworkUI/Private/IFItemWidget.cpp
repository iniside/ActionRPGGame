// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "IFInventoryComponent.h"

void UIFItemWidget::OnSlotCreated(uint8 InNetIndex, uint8 InLocalIndex)
{
	NetIndex = InNetIndex;
	LocalIndex = InLocalIndex;
	//const UIFItemBase* Item = Inventory->GetItem(InLocalIndex);
	//BP_OnItemCreated(const_cast<UIFItemBase*>(Item));
}

void UIFItemWidget::OnItemChanged(uint8 InNetIndex, uint8 InLocalIndex)
{
	const UIFItemBase* Item = Inventory->GetItem(InLocalIndex);
	BP_OnItemChanged(const_cast<UIFItemBase*>(Item));
}
