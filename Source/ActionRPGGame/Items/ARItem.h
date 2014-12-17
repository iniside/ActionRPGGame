// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ARItem.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UARItem : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Item Info")
		FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "Item Info")
		UTexture2D* ItemIcon;

	/*
		This is just for testing (;.
	*/
	UPROPERTY(EditAnywhere, Category = "SubItem")
		TSubclassOf<UObject> SubItem;
};
