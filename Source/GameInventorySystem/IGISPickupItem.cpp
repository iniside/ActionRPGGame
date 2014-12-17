// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "IGISPickupItem.h"

UIGISPickupItem::UIGISPickupItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
FPickupItemContainer IIGISPickupItem::GetItems()
{
	FPickupItemContainer temp;
	return temp;
}
