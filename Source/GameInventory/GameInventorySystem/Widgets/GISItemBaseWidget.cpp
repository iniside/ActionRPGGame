// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"

#include "../GISItemData.h"

#include "GISItemBaseWidget.h"

UGISItemBaseWidget::UGISItemBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemData = nullptr;
}

void UGISItemBaseWidget::InitializeItem()
{

}
FText UGISItemBaseWidget::GetItemName() const
{
	if (ItemData)
	{
		return ItemData->GetItemName();
	}
	return FText::FromString("");
}

UTexture2D* UGISItemBaseWidget::GetIcon() const
{
	if (ItemData)
	{
		return ItemData->GetImage();
	}
	return nullptr;
}

FText UGISItemBaseWidget::GetDescription() const
{
	if (ItemData)
	{
		return ItemData->GetDescription();
	}
	return FText::FromString("");
}