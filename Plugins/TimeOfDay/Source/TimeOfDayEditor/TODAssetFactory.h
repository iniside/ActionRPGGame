// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Factories/Factory.h"
#include "TODAssetFactory.generated.h"

UCLASS()
class UTODAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = ItemsData)
		TSubclassOf<class UTODAsset> ItemsDataClass;

	// UFactory interface

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	// End of UFactory interface
};

