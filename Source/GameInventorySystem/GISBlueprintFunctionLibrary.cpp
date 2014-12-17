// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameInventorySystem.h"
#include "GISItemData.h"
#include "GISBlueprintFunctionLibrary.h"

UGISBlueprintFunctionLibrary::UGISBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

class UGISItemData* UGISBlueprintFunctionLibrary::CreateItem(UObject* WorldContextObject, TSubclassOf<class UGISItemData> ItemClass, APlayerController* OwningPlayer)
{
	return CreateDataItem<UGISItemData>(OwningPlayer, ItemClass);
}