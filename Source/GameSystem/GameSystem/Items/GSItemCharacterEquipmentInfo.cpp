// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSItem.h"
#include "GSEquipmentComponent.h"

#include "Net/UnrealNetwork.h"

#include "GSItemCharacterEquipmentInfo.h"

UGSItemCharacterEquipmentInfo::UGSItemCharacterEquipmentInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UGSItemCharacterEquipmentInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UGSItemCharacterEquipmentInfo, Item, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UGSItemCharacterEquipmentInfo, ItemParts, COND_OwnerOnly);
}

UTexture2D* UGSItemCharacterEquipmentInfo::GetImage()
{
	if (Item.GetDefaultObject())
	{
		return Item.GetDefaultObject()->Icon;
	}
	return nullptr;
}

bool UGSItemCharacterEquipmentInfo::OnItemAddedToSlot()
{
	if (UGSEquipmentComponent* eqComp = Cast<UGSEquipmentComponent>(CurrentInventory))
	{
		if (Item.GetDefaultObject())
		{
			eqComp->SetSkeletalMesh(Item.GetDefaultObject()->MySkeletalMesh
				, MyTag.GetTagName());
			return true;
		}
	}
	return false;
}
bool UGSItemCharacterEquipmentInfo::OnItemRemovedFromSlot()
{
	if (UGSEquipmentComponent* eqComp = Cast<UGSEquipmentComponent>(LastInventory))
	{
		if (Item.GetDefaultObject())
		{
			eqComp->RemoveSkeletalMesh(MyTag.GetTagName());
			return true;
		}
	}
	return false;
}

bool UGSItemCharacterEquipmentInfo::CanItemBeSwapped()
{
	return true;
}