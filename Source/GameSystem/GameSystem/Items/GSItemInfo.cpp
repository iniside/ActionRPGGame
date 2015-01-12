// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSItem.h"
#include "GSEquipmentComponent.h"

//#include "Net/UnrealNetwork.h"

#include "GSItemInfo.h"

UGSItemInfo::UGSItemInfo(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

//void UGSItemInfo::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//}

UTexture2D* UGSItemInfo::GetImage()
{
	return nullptr;
}

bool UGSItemInfo::OnItemAddedToSlot()
{
	return false;
}
bool UGSItemInfo::OnItemRemovedFromSlot()
{
	return false;
}

bool UGSItemInfo::CanItemBeSwapped()
{
	return true;
}