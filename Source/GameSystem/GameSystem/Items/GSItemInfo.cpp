// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSItem.h"
#include "GSEquipmentComponent.h"

#include "Net/UnrealNetwork.h"

#include "GSItemInfo.h"

UGSItemInfo::UGSItemInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UGSItemInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGSItemInfo, Item, COND_OwnerOnly);
}

UTexture2D* UGSItemInfo::GetImage()
{
	if (Item.GetDefaultObject())
	{
		return Item.GetDefaultObject()->Icon;
	}
	return nullptr;
}

bool UGSItemInfo::OnItemAddedToSlot()
{
	if (UGSEquipmentComponent* eqComp = Cast<UGSEquipmentComponent>(CurrentInventory))
	{
		if (Item.GetDefaultObject())
		{
			eqComp->EquiptItem(Item.GetDefaultObject()->MySkeletalMesh);
			return true;
		}
	}
	return false;
}
bool UGSItemInfo::OnItemRemovedFromSlot()
{
	return false;
}

void UGSItemInfo::InputPressed()
{

}
void UGSItemInfo::InputReleased()
{

}

bool UGSItemInfo::CanItemBeSwapped()
{
	return true;
}