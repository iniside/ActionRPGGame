// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"

#include "TODAssetEditor.h"

#include "TODAssetTypeAction.h"

FText FTODAssetTypeAction::GetName() const
{
	return FText::FromString("Time Of Day");
}
FColor FTODAssetTypeAction::GetTypeColor() const
{
	return FColor(255, 0, 0);
}
UClass* FTODAssetTypeAction::GetSupportedClass() const
{
	return UTODAsset::StaticClass();
}
void FTODAssetTypeAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ?
		EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UTODAsset* ItemData = Cast<UTODAsset>(*ObjIt))
		{
			TSharedRef<FTODAssetEditor> NewItemEditor(new FTODAssetEditor());
			NewItemEditor->InitiItemEditor(Mode, EditWithinLevelEditor, ItemData);
		}
	}
}
uint32 FTODAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc;
}
//virtual FText GetAssetDescription(const FAssetData& AssetData) const override;