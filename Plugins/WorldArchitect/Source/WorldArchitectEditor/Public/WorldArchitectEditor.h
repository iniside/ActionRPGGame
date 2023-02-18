// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "IAssetTools.h"

class FWorldArchitectEditorModule : public IModuleInterface
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
	EAssetTypeCategories::Type GenericGraphAssetCategoryBit;
public:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};