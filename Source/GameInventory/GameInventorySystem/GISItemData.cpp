// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "GISItemData.h"

UGISItemData::UGISItemData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bNetAddressable = false;
	bCanStack = false;
	bCanBedropped = false;
	StackCounter = 0;
}
bool UGISItemData::IsNameStableForNetworking() const
{
	return bNetAddressable;
}
void UGISItemData::SetNetAddressable()
{
	bNetAddressable = true;
}

UWorld* UGISItemData::GetWorld() const
{
	if (CurrentWorld)
		return CurrentWorld;
	return nullptr;
}