// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "TimeOfDayEditor.h"
#include "TimeOfDayEditorPrivatePCH.h"
#include "../TODAssetTypeAction.h"
//#include "Editor/DetailCustomizations/Private/CurveStructCustomization.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"
//#include "../CurveColorCustomization.h"

class FTimeOfDayEditor : public ITimeOfDayEditor
{
	TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FTimeOfDayEditor, TimeOfDayEditor )


void FTimeOfDayEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

void FTimeOfDayEditor::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FTODAssetTypeAction));

//	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomPropertyTypeLayout("FRichCurve", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCurveStructCustomization::MakeInstance));
	//PropertyModule.RegisterCustomPropertyTypeLayout("RuntimeCurveLinearColor", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCurveColorCustomization::MakeInstance));
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FTimeOfDayEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	//CreatedAssetTypeActions.Empty();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



