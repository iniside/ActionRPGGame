// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "GISItemData.h"

UGISItemData::UGISItemData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bNetAddressable = false;
}
bool UGISItemData::IsNameStableForNetworking() const
{
	return bNetAddressable;
}