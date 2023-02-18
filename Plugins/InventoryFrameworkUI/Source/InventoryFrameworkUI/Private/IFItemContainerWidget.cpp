// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemContainerWidget.h"
#include "IFInventoryComponent.h"
#include "IFItemWidget.h"

void UIFItemContainerWidget::SetInventory(UIFInventoryComponent* InInventory)
{
	Inventory = InInventory;
}

void UIFItemContainerWidget::CreateInventory()
{
	NativeOnInventoryCreated();
}

void UIFItemContainerWidget::NativeOnInventoryCreated()
{
	BP_OnInventoryCreated();
}