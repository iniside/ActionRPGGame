// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"

#include "ARItem.h"

#include "Net/UnrealNetwork.h"

#include "ARItemInfo.h"

UARItemInfo::UARItemInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UARItemInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UARItemInfo, ItemClass);
	DOREPLIFETIME(UARItemInfo, ItemUpgradeClasses);
}

FText UARItemInfo::GetItemName()
{
	if (ItemClass && ItemClass.GetDefaultObject())
	{
		return ItemClass.GetDefaultObject()->DisplayName;
	}
	return FText::FromString("");
}

UTexture2D* UARItemInfo::GetImage()
{
	if(ItemClass && ItemClass.GetDefaultObject())
	{
		return ItemClass.GetDefaultObject()->ItemIcon;
	}
	return nullptr;
}

int32 UARItemInfo::GetItemTypeID() const { return UARItemInfo::ItemTypeID; }

bool UARItemInfo::IsOfType(int32 ItemTypeIDIn) { return UARItemInfo::ItemTypeID == ItemTypeIDIn; }