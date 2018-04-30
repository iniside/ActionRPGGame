// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WorldArchitectEditor.h"
#include "WorldArchitectEdMode.h"
#include "WALandscapeGraphAssetTypeActions.h"
#define LOCTEXT_NAMESPACE "FWorldArchitectModule"
void FWorldArchitectEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, 
	TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}
void FWorldArchitectEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	GenericGraphAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Landscape")), FText::FromString("Landscape"));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FWALandscapeGraphAssetTypeActions(GenericGraphAssetCategoryBit)));

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FWorldArchitectEdMode>(FWorldArchitectEdMode::EM_WorldArchitectEdModeId, LOCTEXT("WorldArchitectEdModeName", "WorldArchitectEdMode"), FSlateIcon(), true);
}

void FWorldArchitectEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FWorldArchitectEdMode::EM_WorldArchitectEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldArchitectEditorModule, WorldArchitect)