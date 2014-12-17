// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISItemData.h"
#include "ARItemInfo.generated.h"

/*

*/
UCLASS(Blueprintable, BlueprintType)
class UARItemInfo : public UGISItemData
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Class of item. BlueprintReadWrite, meta = (ExposeOnSpawn)
	*/
	UPROPERTY(EditAnywhere, Replicated, Category = "Main ITem")
		TSubclassOf<class UARItem> ItemClass;

	/*
		List of upgrades that should be appiled to item upon creation.
	*/
	UPROPERTY(EditAnywhere, Replicated, Category = "Item Upgrade")
		TArray < TSubclassOf<UObject> >ItemUpgradeClasses;

	virtual FText GetItemName() override;

	virtual UTexture2D* GetImage() override;

	static const int32 ItemTypeID = 1;

	virtual int32 GetItemTypeID() const override;

	virtual bool IsOfType(int32 ItemTypeIDIn) override;
};
